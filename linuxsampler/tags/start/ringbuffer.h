/***************************************************************************
 *                                                                         *
 *   LinuxSampler - modular, streaming capable sampler                     *
 *                                                                         *
 *   Copyright (C) 2003 by Benno Senoner and Christian Schoenebeck         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,                 *
 *   MA  02111-1307  USA                                                   *
 ***************************************************************************/

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define DEFAULT_WRAP_ELEMENTS 1024

#include <string.h>
#include <stdio.h>
#include <asm/atomic.h>

#include <sys/types.h>
#include <pthread.h>

template<class T>
class RingBuffer
{
public:
    RingBuffer (int sz, int wrap_elements = DEFAULT_WRAP_ELEMENTS) {
            int power_of_two;

            this->wrap_elements = wrap_elements;

            for (power_of_two = 1;
                 1<<power_of_two < sz;
                 power_of_two++);

            size = 1<<power_of_two;
            size_mask = size;
            size_mask -= 1;
            atomic_set(&write_ptr, 0);
            atomic_set(&read_ptr, 0);
            buf = new T[size + wrap_elements];
    };

    virtual ~RingBuffer() {
            delete [] buf;
    }

    /**
     * Sets all remaining write space elements to zero. The write pointer
     * will currently not be incremented after, but that might change in
     * future.
     */
    inline void fill_write_space_with_null() {
             int w = atomic_read(&write_ptr),
                 r = atomic_read(&read_ptr);
             memset(get_write_ptr(), 0, write_space_to_end());
             if (r && w >= r) {
               memset(get_buffer_begin(), 0, r - 1);
             }

             // set the wrap space elements to null
             if (wrap_elements) memset(&buf[size], 0, wrap_elements);
           }

    __inline int  read (T *dest, int cnt);
    __inline int  write (T *src, int cnt);
    __inline T *get_buffer_begin();

    __inline T *get_read_ptr(void) {
      return(&buf[atomic_read(&read_ptr)]);
    }

    __inline T *get_write_ptr();
    __inline void increment_read_ptr(int cnt) {
               atomic_set(&read_ptr , (atomic_read(&read_ptr) + cnt) & size_mask);
             }                
    __inline void set_read_ptr(int val) {
               atomic_set(&read_ptr , val);
             }                

    __inline void increment_write_ptr(int cnt) {
               atomic_set(&write_ptr,  (atomic_read(&write_ptr) + cnt) & size_mask);
             }                

    /* this function increments the write_ptr by cnt, if the buffer wraps then
       subtract size from the write_ptr value so that it stays within 0<write_ptr<size
       use this function to increment the write ptr after you filled the buffer
       with a number of elements given by write_space_to_end_with_wrap().
       This ensures that the data that is written to the buffer fills up all
       the wrap space that resides past the regular buffer. The wrap_space is needed for
       interpolation. So that the audio thread sees the ringbuffer like a linear space
       which allows us to use faster routines.
       When the buffer wraps the wrap part is memcpy()ied to the beginning of the buffer
       and the write ptr incremented accordingly.       
    */
    __inline void increment_write_ptr_with_wrap(int cnt) {
               int w=atomic_read(&write_ptr);
               w += cnt;
               if(w >= size) {
                 w -= size;
                 memcpy(&buf[0], &buf[size], w*sizeof(T));
//printf("DEBUG !!!! increment_write_ptr_with_wrap: buffer wrapped, elements wrapped = %d (wrap_elements %d)\n",w,wrap_elements);
               } 
               atomic_set(&write_ptr, w);
             }                

    /* this function returns the available write space in the buffer
       when the read_ptr > write_ptr it returns the space inbetween, otherwise
       when the read_ptr < write_ptr it returns the space between write_ptr and
       the buffer end, including the wrap_space.
       There is an exception to it. When read_ptr <= wrap_elements. In that
       case we return the write space to buffer end (-1) without the wrap_elements,
       this is needed because a subsequent increment_write_ptr which copies the
       data that resides into the wrap space to the beginning of the buffer and increments
       the write_ptr would cause the write_ptr overstepping the read_ptr which would be an error.
       So basically the if(r<=wrap_elements) we return the buffer space to end - 1 which
       ensures that at the next call there will be one element free to write before the buffer wraps
       and usually (except in EOF situations) the next write_space_to_end_with_wrap() will return
       1 + wrap_elements which ensures that the wrap part gets fully filled with data
    */
    __inline int write_space_to_end_with_wrap() {
              int w, r;

              w = atomic_read(&write_ptr);
              r = atomic_read(&read_ptr);
//printf("write_space_to_end: w=%d r=%d\n",w,r);
              if(r > w) {
                //printf("DEBUG: write_space_to_end_with_wrap: r>w r=%d w=%d val=%d\n",r,w,r - w - 1);
                return(r - w - 1);
              }
              if(r <= wrap_elements) {
                //printf("DEBUG: write_space_to_end_with_wrap: ATTENTION r <= wrap_elements: r=%d w=%d val=%d\n",r,w,size - w -1);
                return(size - w -1);
              }
              if(r) {
                //printf("DEBUG: write_space_to_end_with_wrap: r=%d w=%d val=%d\n",r,w,size - w + wrap_elements);
                return(size - w + wrap_elements);
              }
              //printf("DEBUG: write_space_to_end_with_wrap: r=0 w=%d val=%d\n",w,size - w - 1 + wrap_elements);
              return(size - w - 1 + wrap_elements);
            }

           /* this function adjusts the number of elements to write into the ringbuffer
              in a way that the size boundary is avoided and that the wrap space always gets
              entirely filled.
              cnt contains the write_space_to_end_with_wrap() amount while 
              capped_cnt contains a capped amount of samples to read. 
              normally capped_cnt == cnt but in some cases eg when the disk thread needs
              to refill tracks with smaller blocks because lots of streams require immediate
              refill because lots of notes were started simultaneously.
              In that case we set for example capped_cnt to a fixed amount (< cnt, eg 64k),
              which helps to reduce the buffer refill latencies that occur between streams.
              the first if() checks if the current write_ptr + capped_cnt resides within
              the wrap area but is < size+wrap_elements. in that case we cannot return 
              capped_cnt because it would lead to a write_ptr wrapping and only a partial fill
              of wrap space which would lead to errors. So we simply return cnt which ensures
              that the the entire wrap space will get filled correctly.
              In all other cases (which are not problematic because no write_ptr wrapping 
              occurs) we simply return capped_cnt.
           */
    __inline int adjust_write_space_to_avoid_boundary(int cnt, int capped_cnt) {
               int w;
               w = atomic_read(&write_ptr);
               if((w+capped_cnt) >= size && (w+capped_cnt) < (size+wrap_elements)) {
//printf("adjust_write_space_to_avoid_boundary returning cnt = %d\n",cnt);
                 return(cnt);
               }
//printf("adjust_write_space_to_avoid_boundary returning capped_cnt = %d\n",capped_cnt);
               return(capped_cnt);
             }

    __inline int write_space_to_end() {
              int w, r;

              w = atomic_read(&write_ptr);
              r = atomic_read(&read_ptr);
//printf("write_space_to_end: w=%d r=%d\n",w,r);
              if(r > w) return(r - w - 1);
              if(r) return(size - w);
              return(size - w - 1);
            }

    __inline int read_space_to_end() {
              int w, r;

              w = atomic_read(&write_ptr);
              r = atomic_read(&read_ptr);
              if(w >= r) return(w - r);
              return(size - r);
            }
    __inline void init() {
                   atomic_set(&write_ptr, 0);
                   atomic_set(&read_ptr, 0);
                 //  wrap=0;
            }

    int write_space () {
            int w, r;

            w = atomic_read(&write_ptr);
            r = atomic_read(&read_ptr);

            if (w > r) {
                    return ((r - w + size) & size_mask) - 1;
            } else if (w < r) {
                    return (r - w) - 1;
            } else {
                    return size - 1;
            }
    }

    int read_space () {
            int w, r;

            w = atomic_read(&write_ptr);
            r = atomic_read(&read_ptr);

            if (w >= r) {
                    return w - r;
            } else {
                    return (w - r + size) & size_mask;
            }
    }

    int size;
    int wrap_elements;

  protected:
    T *buf;
    atomic_t write_ptr;
    atomic_t read_ptr;
    int size_mask;
};

template<class T> T *
RingBuffer<T>::get_write_ptr (void) {
  return(&buf[atomic_read(&write_ptr)]);
}

template<class T> T *
RingBuffer<T>::get_buffer_begin (void) {
  return(buf);
}



template<class T> int
RingBuffer<T>::read (T *dest, int cnt)

{
        int free_cnt;
        int cnt2;
        int to_read;
        int n1, n2;
        int priv_read_ptr;

        priv_read_ptr=atomic_read(&read_ptr);

        if ((free_cnt = read_space ()) == 0) {
                return 0;
        }

        to_read = cnt > free_cnt ? free_cnt : cnt;
        
        cnt2 = priv_read_ptr + to_read;

        if (cnt2 > size) {
                n1 = size - priv_read_ptr;
                n2 = cnt2 & size_mask;
        } else {
                n1 = to_read;
                n2 = 0;
        }
        
        memcpy (dest, &buf[priv_read_ptr], n1 * sizeof (T));
        priv_read_ptr = (priv_read_ptr + n1) & size_mask;

        if (n2) {
                memcpy (dest+n1, buf, n2 * sizeof (T));
                priv_read_ptr = n2;
        }

        atomic_set(&read_ptr, priv_read_ptr);
        return to_read;
}

template<class T> int
RingBuffer<T>::write (T *src, int cnt)

{
        int free_cnt;
        int cnt2;
        int to_write;
        int n1, n2;
        int priv_write_ptr;

        priv_write_ptr=atomic_read(&write_ptr);

        if ((free_cnt = write_space ()) == 0) {
                return 0;
        }

        to_write = cnt > free_cnt ? free_cnt : cnt;
        
        cnt2 = priv_write_ptr + to_write;

        if (cnt2 > size) {
                n1 = size - priv_write_ptr;
                n2 = cnt2 & size_mask;
        } else {
                n1 = to_write;
                n2 = 0;
        }

        memcpy (&buf[priv_write_ptr], src, n1 * sizeof (T));
        priv_write_ptr = (priv_write_ptr + n1) & size_mask;

        if (n2) {
                memcpy (buf, src+n1, n2 * sizeof (T));
                priv_write_ptr = n2;
        }
        atomic_set(&write_ptr, priv_write_ptr);
        return to_write;
}


#endif /* RINGBUFFER_H */
