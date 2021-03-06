/***************************************************************************
 *                                                                         *
 *   LinuxSampler - modular, streaming capable sampler                     *
 *                                                                         *
 *   Copyright (C) 2003, 2004 by Benno Senoner and Christian Schoenebeck   *
 *   Copyright (C) 2005 Christian Schoenebeck                              *
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

#ifndef __LS_GIG_ENGINEGLOBALS_H__
#define __LS_GIG_ENGINEGLOBALS_H__

#include "Voice.h"
#include "../common/Event.h"

namespace LinuxSampler { namespace gig {

    class Voice;

    /** @brief Voice Stealing Algorithms
     *
     * Enumeration of all possible voice stealing algorithms.
     */
    enum voice_steal_algo_t {
        voice_steal_algo_none,              ///< Voice stealing disabled.
        voice_steal_algo_oldestvoiceonkey,  ///< Try to kill the oldest voice from same key where the new voice should be spawned.
        voice_steal_algo_oldestkey          ///< Try to kill the oldest voice from the oldest active key.
    };

    /** @brief MIDI key runtime informations
     *
     * Reflects runtime informations for one MIDI key.
     */
    struct midi_key_info_t {
        RTList<Voice>*  pActiveVoices;  ///< Contains the active voices associated with the MIDI key.
        bool            KeyPressed;     ///< Is true if the respective MIDI key is currently pressed.
        bool            Active;         ///< If the key contains active voices.
        bool            ReleaseTrigger; ///< If we have to launch release triggered voice(s) when the key is released
        Pool<uint>::Iterator itSelf;    ///< hack to allow fast deallocation of the key from the list of active keys
        RTList<Event>*  pEvents;        ///< Key specific events (only Note-on, Note-off and sustain pedal currently)
        int             VoiceTheftsQueued; ///< Amount of voices postponed due to shortage of voices.
        uint8_t         RoundRobinIndex; ///< For the round robin dimension: current articulation for this key, will be incremented for each note on
        uint8_t         Velocity;       ///< Latest Note-on velocity for this key
        unsigned long   NoteOnTime;     ///< Time for latest Note-on event for this key
    };

}} // namespace LinuxSampler::gig

#endif // __LS_GIG_ENGINEGLOBALS_H__
