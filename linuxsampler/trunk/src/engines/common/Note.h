/*
 * Copyright (c) 2016 - 2017 Christian Schoenebeck
 *
 * http://www.linuxsampler.org
 *
 * This file is part of LinuxSampler and released under the same terms.
 * See README file for details.
 */ 

#ifndef LS_NOTE_H
#define LS_NOTE_H

#include "../../common/Pool.h"
#include "Event.h"

namespace LinuxSampler {

    /**
     * Abstract base class of its deriving @c Note class, this class (NoteBase)
     * is not intended to be instantiated directly. It just provides access to
     * the parts of a Note object which do not depend on any C++ template
     * parameter.
     */
    class NoteBase {
    public:
        int hostKey; ///< Key on which this is @c Note is allocated on. This is usually the note-on event's note number, however in case of a child note this will rather be the parent note's key instead!
        note_id_t parentNoteID; ///< If not null: unique ID of the parent note of this note (see comments of field @c pChildNotes).
        RTList<note_id_t>* pChildNotes; ///< Note ID list of "child" notes of this note. These are special notes that must be released once this note gets released.
        Event cause; ///< Copy of the original event (usually a note-on event) which caused this note.
        event_id_t eventID; ///< Unique ID of the actual original @c Event which caused this note.
        sched_time_t triggerSchedTime; ///< Engine's scheduler time when this note was launched.
        /// Optional synthesis parameters that might be overridden (by calling real-time instrument script functions like change_vol(), change_pitch(), etc.).
        struct  _Override {
            float Volume;       ///< as linear amplification ratio (1.0 being neutral)
            float Pitch;        ///< as linear frequency ratio (1.0 being neutral)
            float Pan;          ///< between -1.0 (most left) and +1.0 (most right) and 0.0 being neutral.
            int64_t PanSources; ///< Might be used for calculating an average pan value in differential way: amount of times the Pan value had been changed and shall be calculated relatively upon.
            float Cutoff;       ///< between 0.0 and 1.0
            float Resonance;    ///< between 0.0 and 1.0
            float Attack;       ///< between 0.0 and 1.0
            float Decay;        ///< between 0.0 and 1.0
            float Release;      ///< between 0.0 and 1.0
            float AmpLFODepth;  ///< between 0.0 and 1.0
            float AmpLFOFreq;   ///< between 0.0 and 1.0
            float PitchLFODepth; ///< between 0.0 and 1.0
            float PitchLFOFreq; ///< between 0.0 and 1.0
        } Override;
        /// Sampler format specific informations and variables.
        union _Format {
            /// Gigasampler/GigaStudio format specifics.
            struct _Gig {
                uint8_t DimMask; ///< May be used to override the Dimension zone to be selected for a new voice: each 1 bit means that respective bit shall be overridden by taking the respective bit from DimBits instead.
                uint8_t DimBits; ///< Used only in conjunction with DimMask: Dimension bits that shall be selected.
            } Gig;
        } Format;
    protected:
        NoteBase() : hostKey(0), parentNoteID(0), pChildNotes(NULL) {
            Override.Volume     = 1.f;
            Override.Pitch      = 1.f;
            Override.Pan        = 0.f;
            Override.PanSources = 0;
            Override.Cutoff     = 1.f;
            Override.Resonance  = 1.f;
            Override.Attack     = 1.f;
            Override.Decay      = 1.f;
            Override.Release    = 1.f;
            Override.AmpLFODepth   = 1.f;
            Override.AmpLFOFreq    = 1.f;
            Override.PitchLFODepth = 1.f;
            Override.PitchLFOFreq  = 1.f;

            Format = _Format();
        }
    };

    /**
     * Contains the voices caused by one specific note, as well as basic
     * informations about the note itself. You can see a Note object as one
     * specific event in time where one or more voices were spawned at the same
     * time and all those voices due to the same cause.
     *
     * For example when you press down and hold the sustain pedal, and then
     * trigger the same note on the keyboard multiple times, for each key
     * strokes a separate Note instance is created.
     *
     * If your instrument contains a real-time instrument script, then that
     * script might also trigger additional voices programmatically (by
     * calling the built-in script function play_note()). Each time the script
     * calls play_note() a new Note instance is created and the script may then
     * further control the voices of specific notes independently from each
     * other. For example for each key stroke on your keyboard the instrument
     * script might trigger 3 additional notes programmatically and assign a
     * different tuning filter parameters for each one of the 3 notes
     * independently.
     */
    template<class V>
    class Note : public NoteBase {
    public:
        RTList<V>* pActiveVoices; ///< Contains the active voices associated with this note.

        Note() : NoteBase(), pActiveVoices(NULL) {}

        virtual ~Note() {
            if (pChildNotes) delete pChildNotes;
            if (pActiveVoices) delete pActiveVoices;
        }

        void init(Pool<V>* pVoicePool, Pool<note_id_t>* pNoteIDPool) {
            if (pActiveVoices) delete pActiveVoices;
            pActiveVoices = new RTList<V>(pVoicePool);
            if (pChildNotes) delete pChildNotes;
            pChildNotes = new RTList<note_id_t>(pNoteIDPool);
        }

        void reset() {
            hostKey = 0;
            parentNoteID = 0;
            if (pChildNotes)
                pChildNotes->clear();
            cause = Event();
            eventID = 0;
            Override.Volume     = 1.f;
            Override.Pitch      = 1.f;
            Override.Pan        = 0.f;
            Override.PanSources = 0;
            Override.Cutoff     = 1.f;
            Override.Resonance  = 1.f;
            Override.Attack     = 1.f;
            Override.Decay      = 1.f;
            Override.Release    = 1.f;
            Override.AmpLFODepth   = 1.f;
            Override.AmpLFOFreq    = 1.f;
            Override.PitchLFODepth = 1.f;
            Override.PitchLFOFreq  = 1.f;
            Format = _Format();
            if (pActiveVoices) {
                typename RTList<V>::Iterator itVoice = pActiveVoices->first();
                typename RTList<V>::Iterator itVoicesEnd = pActiveVoices->end();
                for (; itVoice != itVoicesEnd; ++itVoice) { // iterate through all voices on this key
                    itVoice->VoiceFreed();
                }
                pActiveVoices->clear();
            }
        }
    };

} // namespace LinuxSampler

#endif // LS_NOTE_H
