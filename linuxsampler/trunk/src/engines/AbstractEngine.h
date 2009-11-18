/***************************************************************************
 *                                                                         *
 *   LinuxSampler - modular, streaming capable sampler                     *
 *                                                                         *
 *   Copyright (C) 2003,2004 by Benno Senoner and Christian Schoenebeck    *
 *   Copyright (C) 2005-2009 Christian Schoenebeck                         *
 *   Copyright (C) 2009 Grigor Iliev                                       *
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

#ifndef __LS_ABSTRACTENGINE_H__
#define	__LS_ABSTRACTENGINE_H__

#include "Engine.h"
#include "../common/ArrayList.h"
#include "../common/atomic.h"
#include "../common/ConditionServer.h"
#include "../common/Pool.h"
#include "../common/RingBuffer.h"
#include "../drivers/audio/AudioOutputDevice.h"
#include "common/Event.h"

namespace LinuxSampler {

    class AbstractEngineChannel;

    class AbstractEngine: public Engine {

        public:
            enum Format { GIG = 1, SF2, SFZ };
            static String GetFormatString(Format f);
            static AbstractEngine* AcquireEngine(AbstractEngineChannel* pChannel, AudioOutputDevice* pDevice);
            static void FreeEngine(AbstractEngineChannel* pChannel, AudioOutputDevice* pDevice);

            AbstractEngine();
            virtual ~AbstractEngine();

            // implementation of abstract methods derived from class 'LinuxSampler::Engine'
            virtual void   SendSysex(void* pData, uint Size, MidiInputPort* pSender);
            virtual void   Reset();
            virtual void   Enable();
            virtual void   Disable();
            virtual uint   VoiceCount();
            virtual uint   VoiceCountMax();
            virtual String EngineName();

            virtual Format GetEngineFormat() = 0;
            virtual void   Connect(AudioOutputDevice* pAudioOut) = 0;
            virtual void   DisableAndLock();

            void SetVoiceCount(uint Count);// Simple array wrapper just to make sure memory is freed
            // when liblinuxsampler is unloaded
            class FloatTable {
            private:
                const float* array;
            public:
                FloatTable(const float* array) : array(array) { }
                ~FloatTable() { delete[] array; }
                const float& operator[](int i) const { return array[i]; }
            };

            static const FloatTable VolumeCurve;    ///< Table that maps volume control change values 0..127 to amplitude. Unity gain is at 90.
            static const FloatTable PanCurve;       ///< Table that maps pan control change values 0..128 to right channel amplitude. Unity gain is at 64 (center).
            static const FloatTable CrossfadeCurve; ///< Table that maps crossfade control change values 0..127 to amplitude. Unity gain is at 127.

            AudioOutputDevice* pAudioOutputDevice;

            friend class AbstractVoice;
            friend class AbstractEngineChannel;
            template<class V, class R, class I> friend class EngineChannelBase;
            template<class EC, class R, class S, class D> friend class VoiceBase;

        protected:
            ArrayList<EngineChannel*>  engineChannels; ///< All engine channels of a Engine instance.
            ConditionServer            EngineDisabled;
            int8_t                     ScaleTuning[12];    ///< contains optional detune factors (-64..+63 cents) for all 12 semitones of an octave
            RingBuffer<Event,false>*   pEventQueue;        ///< Input event queue for engine global events (e.g. SysEx messages).
            EventGenerator*            pEventGenerator;
            RTList<Event>*             pGlobalEvents;         ///< All engine global events for the current audio fragment (usually only SysEx messages).
            Pool<Event>*               pEventPool;            ///< Contains all Event objects that can be used.
            RingBuffer<uint8_t,false>* pSysexBuffer;          ///< Input buffer for MIDI system exclusive messages.
            uint                       SampleRate;            ///< Sample rate of the engines output audio signal (in Hz)
            uint                       MaxSamplesPerCycle;    ///< Size of each audio output buffer
            unsigned long              FrameTime;             ///< Time in frames of the start of the current audio fragment
            int                        ActiveVoiceCountMax;   ///< the maximum voice usage since application start
            atomic_t                   ActiveVoiceCount;      ///< number of currently active voices
            int                        VoiceSpawnsLeft;       ///< We only allow CONFIG_MAX_VOICES voices to be spawned per audio fragment, we use this variable to ensure this limit.

            void RouteAudio(EngineChannel* pEngineChannel, uint Samples);
            void ClearEventLists();
            void ImportEvents(uint Samples);
            void ProcessSysex(Pool<Event>::Iterator& itSysexEvent);
            void ResetScaleTuning();
            void ProcessPitchbend(AbstractEngineChannel* pEngineChannel, Pool<Event>::Iterator& itPitchbendEvent);

            void ProcessFxSendControllers (
                AbstractEngineChannel*  pEngineChannel,
                Pool<Event>::Iterator&  itControlChangeEvent
            );

            uint8_t GSCheckSum(const RingBuffer<uint8_t,false>::NonVolatileReader AddrReader, uint DataSize);

            virtual void ResetInternal() = 0;
            virtual void KillAllVoices(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itKillEvent) = 0;
            virtual void ProcessNoteOn(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itNoteOnEvent) = 0;
            virtual void ProcessNoteOff(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itNoteOffEvent) = 0;
            virtual void ProcessControlChange(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itControlChangeEvent) = 0;
            virtual int  GetMinFadeOutSamples() = 0;

        private:
            static std::map<Format, std::map<AudioOutputDevice*,AbstractEngine*> > engines;

            static float* InitVolumeCurve();
            static float* InitPanCurve();
            static float* InitCrossfadeCurve();
            static float* InitCurve(const float* segments, int size = 128);

            void AdjustScale(int8_t ScaleTunes[12]);
    };

} // namespace LinuxSampler

#endif	/* __LS_ABSTRACTENGINE_H__ */
