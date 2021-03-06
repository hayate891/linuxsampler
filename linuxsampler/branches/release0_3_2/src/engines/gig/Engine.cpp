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

#include <sstream>
#include "DiskThread.h"
#include "Voice.h"
#include "EGADSR.h"
#include "../EngineFactory.h"

#include "Engine.h"

#if defined(__APPLE__)
# include <stdlib.h>
#else
# include <malloc.h>
#endif

namespace LinuxSampler { namespace gig {

    InstrumentResourceManager Engine::instruments;

    std::map<AudioOutputDevice*,Engine*> Engine::engines;

    /**
     * Get a gig::Engine object for the given gig::EngineChannel and the
     * given AudioOutputDevice. All engine channels which are connected to
     * the same audio output device will use the same engine instance. This
     * method will be called by a gig::EngineChannel whenever it's
     * connecting to a audio output device.
     *
     * @param pChannel - engine channel which acquires an engine object
     * @param pDevice  - the audio output device \a pChannel is connected to
     */
    Engine* Engine::AcquireEngine(LinuxSampler::gig::EngineChannel* pChannel, AudioOutputDevice* pDevice) {
        Engine* pEngine = NULL;
        // check if there's already an engine for the given audio output device
        if (engines.count(pDevice)) {
            dmsg(4,("Using existing gig::Engine.\n"));
            pEngine = engines[pDevice];
        } else { // create a new engine (and disk thread) instance for the given audio output device
            dmsg(4,("Creating new gig::Engine.\n"));
            pEngine = (Engine*) EngineFactory::Create("gig");
            pEngine->Connect(pDevice);
            engines[pDevice] = pEngine;
        }
        // register engine channel to the engine instance
        pEngine->engineChannels.add(pChannel);
        // remember index in the ArrayList
        pChannel->iEngineIndexSelf = pEngine->engineChannels.size() - 1;
        dmsg(4,("This gig::Engine has now %d EngineChannels.\n",pEngine->engineChannels.size()));
        return pEngine;
    }

    /**
     * Once an engine channel is disconnected from an audio output device,
     * it wil immediately call this method to unregister itself from the
     * engine instance and if that engine instance is not used by any other
     * engine channel anymore, then that engine instance will be destroyed.
     *
     * @param pChannel - engine channel which wants to disconnect from it's
     *                   engine instance
     * @param pDevice  - audio output device \a pChannel was connected to
     */
    void Engine::FreeEngine(LinuxSampler::gig::EngineChannel* pChannel, AudioOutputDevice* pDevice) {
        dmsg(4,("Disconnecting EngineChannel from gig::Engine.\n"));
        Engine* pEngine = engines[pDevice];
        // unregister EngineChannel from the Engine instance
        pEngine->engineChannels.remove(pChannel);
        // if the used Engine instance is not used anymore, then destroy it
        if (pEngine->engineChannels.empty()) {
            pDevice->Disconnect(pEngine);
            engines.erase(pDevice);
            delete pEngine;
            dmsg(4,("Destroying gig::Engine.\n"));
        }
        else dmsg(4,("This gig::Engine has now %d EngineChannels.\n",pEngine->engineChannels.size()));
    }

    /**
     * Constructor
     */
    Engine::Engine() {
        pAudioOutputDevice = NULL;
        pDiskThread        = NULL;
        pEventGenerator    = NULL;
        pSysexBuffer       = new RingBuffer<uint8_t>(CONFIG_SYSEX_BUFFER_SIZE, 0);
        pEventQueue        = new RingBuffer<Event>(CONFIG_MAX_EVENTS_PER_FRAGMENT, 0);
        pEventPool         = new Pool<Event>(CONFIG_MAX_EVENTS_PER_FRAGMENT);
        pVoicePool         = new Pool<Voice>(CONFIG_MAX_VOICES);
        pVoiceStealingQueue = new RTList<Event>(pEventPool);
        pGlobalEvents      = new RTList<Event>(pEventPool);
        for (RTList<Voice>::Iterator iterVoice = pVoicePool->allocAppend(); iterVoice == pVoicePool->last(); iterVoice = pVoicePool->allocAppend()) {
            iterVoice->SetEngine(this);
        }
        pVoicePool->clear();

        pSynthesisParameters[0] = NULL; // we allocate when an audio device is connected
        pBasicFilterParameters  = NULL;
        pMainFilterParameters   = NULL;

        ResetInternal();
        ResetScaleTuning();
    }

    /**
     * Destructor
     */
    Engine::~Engine() {
        if (pDiskThread) {
            dmsg(1,("Stopping disk thread..."));
            pDiskThread->StopThread();
            delete pDiskThread;
            dmsg(1,("OK\n"));
        }
        if (pEventQueue) delete pEventQueue;
        if (pEventPool)  delete pEventPool;
        if (pVoicePool) {
            pVoicePool->clear();
            delete pVoicePool;
        }
        if (pEventGenerator) delete pEventGenerator;
        if (pMainFilterParameters) delete[] pMainFilterParameters;
        if (pBasicFilterParameters) delete[] pBasicFilterParameters;
        if (pSynthesisParameters[0]) free(pSynthesisParameters[0]);
        if (pVoiceStealingQueue) delete pVoiceStealingQueue;
        if (pSysexBuffer) delete pSysexBuffer;
        EngineFactory::Destroy(this);
    }

    void Engine::Enable() {
        dmsg(3,("gig::Engine: enabling\n"));
        EngineDisabled.PushAndUnlock(false, 2); // set condition object 'EngineDisabled' to false (wait max. 2s)
        dmsg(3,("gig::Engine: enabled (val=%d)\n", EngineDisabled.GetUnsafe()));
    }

    void Engine::Disable() {
        dmsg(3,("gig::Engine: disabling\n"));
        bool* pWasDisabled = EngineDisabled.PushAndUnlock(true, 2); // wait max. 2s
        if (!pWasDisabled) dmsg(3,("gig::Engine warning: Timeout waiting to disable engine.\n"));
    }

    void Engine::DisableAndLock() {
        dmsg(3,("gig::Engine: disabling\n"));
        bool* pWasDisabled = EngineDisabled.Push(true, 2); // wait max. 2s
        if (!pWasDisabled) dmsg(3,("gig::Engine warning: Timeout waiting to disable engine.\n"));
    }

    /**
     *  Reset all voices and disk thread and clear input event queue and all
     *  control and status variables.
     */
    void Engine::Reset() {
        DisableAndLock();
        ResetInternal();
        ResetScaleTuning();
        Enable();
    }

    /**
     *  Reset all voices and disk thread and clear input event queue and all
     *  control and status variables. This method is not thread safe!
     */
    void Engine::ResetInternal() {
        ActiveVoiceCount    = 0;
        ActiveVoiceCountMax = 0;

        // reset voice stealing parameters
        pVoiceStealingQueue->clear();
        itLastStolenVoice          = RTList<Voice>::Iterator();
        itLastStolenVoiceGlobally  = RTList<Voice>::Iterator();
        iuiLastStolenKey           = RTList<uint>::Iterator();
        iuiLastStolenKeyGlobally   = RTList<uint>::Iterator();
        pLastStolenChannel         = NULL;

        // reset all voices
        for (RTList<Voice>::Iterator iterVoice = pVoicePool->allocAppend(); iterVoice == pVoicePool->last(); iterVoice = pVoicePool->allocAppend()) {
            iterVoice->Reset();
        }
        pVoicePool->clear();

        // reset disk thread
        if (pDiskThread) pDiskThread->Reset();

        // delete all input events
        pEventQueue->init();
    }

    /**
     * Reset to normal, chromatic scale (means equal tempered).
     */
    void Engine::ResetScaleTuning() {
        memset(&ScaleTuning[0], 0x00, 12);
    }

    /**
     * Connect this engine instance with the given audio output device.
     * This method will be called when an Engine instance is created.
     * All of the engine's data structures which are dependant to the used
     * audio output device / driver will be (re)allocated and / or
     * adjusted appropriately.
     *
     * @param pAudioOut - audio output device to connect to
     */
    void Engine::Connect(AudioOutputDevice* pAudioOut) {
        pAudioOutputDevice = pAudioOut;

        ResetInternal();

        // inform audio driver for the need of two channels
        try {
            pAudioOutputDevice->AcquireChannels(2); // gig engine only stereo
        }
        catch (AudioOutputException e) {
            String msg = "Audio output device unable to provide 2 audio channels, cause: " + e.Message();
            throw LinuxSamplerException(msg);
        }

        this->MaxSamplesPerCycle = pAudioOutputDevice->MaxSamplesPerCycle();
        this->SampleRate         = pAudioOutputDevice->SampleRate();

        // FIXME: audio drivers with varying fragment sizes might be a problem here
        MaxFadeOutPos = MaxSamplesPerCycle - int(double(SampleRate) * CONFIG_EG_MIN_RELEASE_TIME) - 1;
        if (MaxFadeOutPos < 0)
            throw LinuxSamplerException("CONFIG_EG_MIN_RELEASE_TIME too big for current audio fragment size / sampling rate!");

        // (re)create disk thread
        if (this->pDiskThread) {
            dmsg(1,("Stopping disk thread..."));
            this->pDiskThread->StopThread();
            delete this->pDiskThread;
            dmsg(1,("OK\n"));
        }
        this->pDiskThread = new DiskThread(((pAudioOut->MaxSamplesPerCycle() << CONFIG_MAX_PITCH) << 1) + 6); //FIXME: assuming stereo
        if (!pDiskThread) {
            dmsg(0,("gig::Engine  new diskthread = NULL\n"));
            exit(EXIT_FAILURE);
        }

        for (RTList<Voice>::Iterator iterVoice = pVoicePool->allocAppend(); iterVoice == pVoicePool->last(); iterVoice = pVoicePool->allocAppend()) {
            iterVoice->pDiskThread = this->pDiskThread;
            dmsg(3,("d"));
        }
        pVoicePool->clear();

        // (re)create event generator
        if (pEventGenerator) delete pEventGenerator;
        pEventGenerator = new EventGenerator(pAudioOut->SampleRate());

        // (re)allocate synthesis parameter matrix
        if (pSynthesisParameters[0]) free(pSynthesisParameters[0]);

        #if defined(__APPLE__)
        pSynthesisParameters[0] = (float *) malloc(Event::destination_count * sizeof(float) * pAudioOut->MaxSamplesPerCycle());
        #else
        pSynthesisParameters[0] = (float *) memalign(16,(Event::destination_count * sizeof(float) * pAudioOut->MaxSamplesPerCycle()));
        #endif
        for (int dst = 1; dst < Event::destination_count; dst++)
            pSynthesisParameters[dst] = pSynthesisParameters[dst - 1] + pAudioOut->MaxSamplesPerCycle();

        // (re)allocate biquad filter parameter sequence
        if (pBasicFilterParameters) delete[] pBasicFilterParameters;
        if (pMainFilterParameters)  delete[] pMainFilterParameters;
        pBasicFilterParameters = new biquad_param_t[pAudioOut->MaxSamplesPerCycle()];
        pMainFilterParameters  = new biquad_param_t[pAudioOut->MaxSamplesPerCycle()];

        dmsg(1,("Starting disk thread..."));
        pDiskThread->StartThread();
        dmsg(1,("OK\n"));

        for (RTList<Voice>::Iterator iterVoice = pVoicePool->allocAppend(); iterVoice == pVoicePool->last(); iterVoice = pVoicePool->allocAppend()) {
            if (!iterVoice->pDiskThread) {
                dmsg(0,("Engine -> voice::trigger: !pDiskThread\n"));
                exit(EXIT_FAILURE);
            }
        }
    }

    /**
     * Clear all engine global event lists.
     */
    void Engine::ClearEventLists() {
        pGlobalEvents->clear();
    }

    /**
     * Copy all events from the engine's global input queue buffer to the
     * engine's internal event list. This will be done at the beginning of
     * each audio cycle (that is each RenderAudio() call) to distinguish
     * all global events which have to be processed in the current audio
     * cycle. These events are usually just SysEx messages. Every
     * EngineChannel has it's own input event queue buffer and event list
     * to handle common events like NoteOn, NoteOff and ControlChange
     * events.
     *
     * @param Samples - number of sample points to be processed in the
     *                  current audio cycle
     */
    void Engine::ImportEvents(uint Samples) {
        RingBuffer<Event>::NonVolatileReader eventQueueReader = pEventQueue->get_non_volatile_reader();
        Event* pEvent;
        while (true) {
            // get next event from input event queue
            if (!(pEvent = eventQueueReader.pop())) break;
            // if younger event reached, ignore that and all subsequent ones for now
            if (pEvent->FragmentPos() >= Samples) {
                eventQueueReader--;
                dmsg(2,("Younger Event, pos=%d ,Samples=%d!\n",pEvent->FragmentPos(),Samples));
                pEvent->ResetFragmentPos();
                break;
            }
            // copy event to internal event list
            if (pGlobalEvents->poolIsEmpty()) {
                dmsg(1,("Event pool emtpy!\n"));
                break;
            }
            *pGlobalEvents->allocAppend() = *pEvent;
        }
        eventQueueReader.free(); // free all copied events from input queue
    }

    /**
     *  Let this engine proceed to render the given amount of sample points. The
     *  calculated audio data of all voices of this engine will be placed into
     *  the engine's audio sum buffer which has to be copied and eventually be
     *  converted to the appropriate value range by the audio output class (e.g.
     *  AlsaIO or JackIO) right after.
     *
     *  @param Samples - number of sample points to be rendered
     *  @returns       0 on success
     */
    int Engine::RenderAudio(uint Samples) {
        dmsg(5,("RenderAudio(Samples=%d)\n", Samples));

        // return if engine disabled
        if (EngineDisabled.Pop()) {
            dmsg(5,("gig::Engine: engine disabled (val=%d)\n",EngineDisabled.GetUnsafe()));
            return 0;
        }

        // update time of start and end of this audio fragment (as events' time stamps relate to this)
        pEventGenerator->UpdateFragmentTime(Samples);

        // We only allow a maximum of CONFIG_MAX_VOICES voices to be spawned
        // in each audio fragment. All subsequent request for spawning new
        // voices in the same audio fragment will be ignored.
        VoiceSpawnsLeft = CONFIG_MAX_VOICES;

        // get all events from the engine's global input event queue which belong to the current fragment
        // (these are usually just SysEx messages)
        ImportEvents(Samples);

        // process engine global events (these are currently only MIDI System Exclusive messages)
        {
            RTList<Event>::Iterator itEvent = pGlobalEvents->first();
            RTList<Event>::Iterator end     = pGlobalEvents->end();
            for (; itEvent != end; ++itEvent) {
                switch (itEvent->Type) {
                    case Event::type_sysex:
                        dmsg(5,("Engine: Sysex received\n"));
                        ProcessSysex(itEvent);
                        break;
                }
            }
        }

        // reset internal voice counter (just for statistic of active voices)
        ActiveVoiceCountTemp = 0;

        // handle events on all engine channels
        for (int i = 0; i < engineChannels.size(); i++) {
            if (!engineChannels[i]->pInstrument) continue; // ignore if no instrument loaded
            ProcessEvents(engineChannels[i], Samples);
        }

        // render all 'normal', active voices on all engine channels
        for (int i = 0; i < engineChannels.size(); i++) {
            if (!engineChannels[i]->pInstrument) continue; // ignore if no instrument loaded
            RenderActiveVoices(engineChannels[i], Samples);
        }

        // now that all ordinary voices on ALL engine channels are rendered, render new stolen voices
        RenderStolenVoices(Samples);

        // handle cleanup on all engine channels for the next audio fragment
        for (int i = 0; i < engineChannels.size(); i++) {
            if (!engineChannels[i]->pInstrument) continue; // ignore if no instrument loaded
            PostProcess(engineChannels[i]);
        }


        // empty the engine's event list for the next audio fragment
        ClearEventLists();

        // reset voice stealing for the next audio fragment
        pVoiceStealingQueue->clear();

        // just some statistics about this engine instance
        ActiveVoiceCount = ActiveVoiceCountTemp;
        if (ActiveVoiceCount > ActiveVoiceCountMax) ActiveVoiceCountMax = ActiveVoiceCount;

        FrameTime += Samples;

        return 0;
    }

    /**
     * Dispatch and handle all events in this audio fragment for the given
     * engine channel.
     *
     * @param pEngineChannel - engine channel on which events should be
     *                         processed
     * @param Samples        - amount of sample points to be processed in
     *                         this audio fragment cycle
     */
    void Engine::ProcessEvents(EngineChannel* pEngineChannel, uint Samples) {
        // get all events from the engine channels's input event queue which belong to the current fragment
        // (these are the common events like NoteOn, NoteOff, ControlChange, etc.)
        pEngineChannel->ImportEvents(Samples);

        // process events
        {
            RTList<Event>::Iterator itEvent = pEngineChannel->pEvents->first();
            RTList<Event>::Iterator end     = pEngineChannel->pEvents->end();
            for (; itEvent != end; ++itEvent) {
                switch (itEvent->Type) {
                    case Event::type_note_on:
                        dmsg(5,("Engine: Note on received\n"));
                        ProcessNoteOn((EngineChannel*)itEvent->pEngineChannel, itEvent);
                        break;
                    case Event::type_note_off:
                        dmsg(5,("Engine: Note off received\n"));
                        ProcessNoteOff((EngineChannel*)itEvent->pEngineChannel, itEvent);
                        break;
                    case Event::type_control_change:
                        dmsg(5,("Engine: MIDI CC received\n"));
                        ProcessControlChange((EngineChannel*)itEvent->pEngineChannel, itEvent);
                        break;
                    case Event::type_pitchbend:
                        dmsg(5,("Engine: Pitchbend received\n"));
                        ProcessPitchbend((EngineChannel*)itEvent->pEngineChannel, itEvent);
                        break;
                }
            }
        }

        // reset voice stealing for the next engine channel (or next audio fragment)
        itLastStolenVoice         = RTList<Voice>::Iterator();
        itLastStolenVoiceGlobally = RTList<Voice>::Iterator();
        iuiLastStolenKey          = RTList<uint>::Iterator();
        iuiLastStolenKeyGlobally  = RTList<uint>::Iterator();
        pLastStolenChannel        = NULL;
    }

    /**
     * Render all 'normal' voices (that is voices which were not stolen in
     * this fragment) on the given engine channel.
     *
     * @param pEngineChannel - engine channel on which audio should be
     *                         rendered
     * @param Samples        - amount of sample points to be rendered in
     *                         this audio fragment cycle
     */
    void Engine::RenderActiveVoices(EngineChannel* pEngineChannel, uint Samples) {
        RTList<uint>::Iterator iuiKey = pEngineChannel->pActiveKeys->first();
        RTList<uint>::Iterator end    = pEngineChannel->pActiveKeys->end();
        while (iuiKey != end) { // iterate through all active keys
            midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[*iuiKey];
            ++iuiKey;

            RTList<Voice>::Iterator itVoice     = pKey->pActiveVoices->first();
            RTList<Voice>::Iterator itVoicesEnd = pKey->pActiveVoices->end();
            for (; itVoice != itVoicesEnd; ++itVoice) { // iterate through all voices on this key
                // now render current voice
                itVoice->Render(Samples);
                if (itVoice->IsActive()) ActiveVoiceCountTemp++; // still active
                else { // voice reached end, is now inactive
                    FreeVoice(pEngineChannel, itVoice); // remove voice from the list of active voices
                }
            }
        }
    }

    /**
     * Render all stolen voices (only voices which were stolen in this
     * fragment) on the given engine channel. Stolen voices are rendered
     * after all normal voices have been rendered; this is needed to render
     * audio of those voices which were selected for voice stealing until
     * the point were the stealing (that is the take over of the voice)
     * actually happened.
     *
     * @param pEngineChannel - engine channel on which audio should be
     *                         rendered
     * @param Samples        - amount of sample points to be rendered in
     *                         this audio fragment cycle
     */
    void Engine::RenderStolenVoices(uint Samples) {
        RTList<Event>::Iterator itVoiceStealEvent = pVoiceStealingQueue->first();
        RTList<Event>::Iterator end               = pVoiceStealingQueue->end();
        for (; itVoiceStealEvent != end; ++itVoiceStealEvent) {
            EngineChannel* pEngineChannel = (EngineChannel*) itVoiceStealEvent->pEngineChannel;
            Pool<Voice>::Iterator itNewVoice =
                LaunchVoice(pEngineChannel, itVoiceStealEvent, itVoiceStealEvent->Param.Note.Layer, itVoiceStealEvent->Param.Note.ReleaseTrigger, false, false);
            if (itNewVoice) {
                itNewVoice->Render(Samples);
                if (itNewVoice->IsActive()) ActiveVoiceCountTemp++; // still active
                else { // voice reached end, is now inactive
                    FreeVoice(pEngineChannel, itNewVoice); // remove voice from the list of active voices
                }
            }
            else dmsg(1,("gig::Engine: ERROR, voice stealing didn't work out!\n"));

            // we need to clear the key's event list explicitly here in case key was never active
            midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[itVoiceStealEvent->Param.Note.Key];
            pKey->VoiceTheftsQueued--;
            if (!pKey->Active && !pKey->VoiceTheftsQueued) pKey->pEvents->clear();
        }
    }

    /**
     * Free all keys which have turned inactive in this audio fragment, from
     * the list of active keys and clear all event lists on that engine
     * channel.
     *
     * @param pEngineChannel - engine channel to cleanup
     */
    void Engine::PostProcess(EngineChannel* pEngineChannel) {
        // free all keys which have no active voices left
        {
            RTList<uint>::Iterator iuiKey = pEngineChannel->pActiveKeys->first();
            RTList<uint>::Iterator end    = pEngineChannel->pActiveKeys->end();
            while (iuiKey != end) { // iterate through all active keys
                midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[*iuiKey];
                ++iuiKey;
                if (pKey->pActiveVoices->isEmpty()) FreeKey(pEngineChannel, pKey);
                #if CONFIG_DEVMODE
                else { // just a sanity check for debugging
                    RTList<Voice>::Iterator itVoice     = pKey->pActiveVoices->first();
                    RTList<Voice>::Iterator itVoicesEnd = pKey->pActiveVoices->end();
                    for (; itVoice != itVoicesEnd; ++itVoice) { // iterate through all voices on this key
                        if (itVoice->itKillEvent) {
                            dmsg(1,("gig::Engine: ERROR, killed voice survived !!!\n"));
                        }
                    }
                }
                #endif // CONFIG_DEVMODE
            }
        }

        // empty the engine channel's own event lists
        pEngineChannel->ClearEventLists();
    }

    /**
     *  Will be called by the MIDI input device whenever a MIDI system
     *  exclusive message has arrived.
     *
     *  @param pData - pointer to sysex data
     *  @param Size  - lenght of sysex data (in bytes)
     */
    void Engine::SendSysex(void* pData, uint Size) {
        Event event             = pEventGenerator->CreateEvent();
        event.Type              = Event::type_sysex;
        event.Param.Sysex.Size  = Size;
        event.pEngineChannel    = NULL; // as Engine global event
        if (pEventQueue->write_space() > 0) {
            if (pSysexBuffer->write_space() >= Size) {
                // copy sysex data to input buffer
                uint toWrite = Size;
                uint8_t* pPos = (uint8_t*) pData;
                while (toWrite) {
                    const uint writeNow = RTMath::Min(toWrite, pSysexBuffer->write_space_to_end());
                    pSysexBuffer->write(pPos, writeNow);
                    toWrite -= writeNow;
                    pPos    += writeNow;

                }
                // finally place sysex event into input event queue
                pEventQueue->push(&event);
            }
            else dmsg(1,("Engine: Sysex message too large (%d byte) for input buffer (%d byte)!",Size,CONFIG_SYSEX_BUFFER_SIZE));
        }
        else dmsg(1,("Engine: Input event queue full!"));
    }

    /**
     *  Assigns and triggers a new voice for the respective MIDI key.
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param itNoteOnEvent - key, velocity and time stamp of the event
     */
    void Engine::ProcessNoteOn(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itNoteOnEvent) {

        const int key = itNoteOnEvent->Param.Note.Key;

        // Change key dimension value if key is in keyswitching area
        {
            const ::gig::Instrument* pInstrument = pEngineChannel->pInstrument;
            if (key >= pInstrument->DimensionKeyRange.low && key <= pInstrument->DimensionKeyRange.high)
                pEngineChannel->CurrentKeyDimension = ((key - pInstrument->DimensionKeyRange.low) * 128) /
                    (pInstrument->DimensionKeyRange.high - pInstrument->DimensionKeyRange.low + 1);
        }

        midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[key];

        pKey->KeyPressed = true; // the MIDI key was now pressed down
        pKey->Velocity   = itNoteOnEvent->Param.Note.Velocity;
        pKey->NoteOnTime = FrameTime + itNoteOnEvent->FragmentPos(); // will be used to calculate note length

        // cancel release process of voices on this key if needed
        if (pKey->Active && !pEngineChannel->SustainPedal) {
            RTList<Event>::Iterator itCancelReleaseEvent = pKey->pEvents->allocAppend();
            if (itCancelReleaseEvent) {
                *itCancelReleaseEvent = *itNoteOnEvent;                  // copy event
                itCancelReleaseEvent->Type = Event::type_cancel_release; // transform event type
            }
            else dmsg(1,("Event pool emtpy!\n"));
        }

        // move note on event to the key's own event list
        RTList<Event>::Iterator itNoteOnEventOnKeyList = itNoteOnEvent.moveToEndOf(pKey->pEvents);

        // allocate and trigger new voice(s) for the key
        {
            // first, get total amount of required voices (dependant on amount of layers)
            ::gig::Region* pRegion = pEngineChannel->pInstrument->GetRegion(itNoteOnEventOnKeyList->Param.Note.Key);
            if (pRegion) {
                int voicesRequired = pRegion->Layers;
                // now launch the required amount of voices
                for (int i = 0; i < voicesRequired; i++)
                    LaunchVoice(pEngineChannel, itNoteOnEventOnKeyList, i, false, true, true);
            }
        }

        // if neither a voice was spawned or postponed then remove note on event from key again
        if (!pKey->Active && !pKey->VoiceTheftsQueued)
            pKey->pEvents->free(itNoteOnEventOnKeyList);

        pKey->RoundRobinIndex++;
    }

    /**
     *  Releases the voices on the given key if sustain pedal is not pressed.
     *  If sustain is pressed, the release of the note will be postponed until
     *  sustain pedal will be released or voice turned inactive by itself (e.g.
     *  due to completion of sample playback).
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param itNoteOffEvent - key, velocity and time stamp of the event
     */
    void Engine::ProcessNoteOff(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itNoteOffEvent) {
        midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[itNoteOffEvent->Param.Note.Key];

        pKey->KeyPressed = false; // the MIDI key was now released

        // release voices on this key if needed
        if (pKey->Active && !pEngineChannel->SustainPedal) {
            itNoteOffEvent->Type = Event::type_release; // transform event type

            // move event to the key's own event list
            RTList<Event>::Iterator itNoteOffEventOnKeyList = itNoteOffEvent.moveToEndOf(pKey->pEvents);

            // spawn release triggered voice(s) if needed
            if (pKey->ReleaseTrigger) {
                // first, get total amount of required voices (dependant on amount of layers)
                ::gig::Region* pRegion = pEngineChannel->pInstrument->GetRegion(itNoteOffEventOnKeyList->Param.Note.Key);
                if (pRegion) {
                    int voicesRequired = pRegion->Layers;

                    // MIDI note-on velocity is used instead of note-off velocity
                    itNoteOffEventOnKeyList->Param.Note.Velocity = pKey->Velocity;

                    // now launch the required amount of voices
                    for (int i = 0; i < voicesRequired; i++)
                        LaunchVoice(pEngineChannel, itNoteOffEventOnKeyList, i, true, false, false); //FIXME: for the moment we don't perform voice stealing for release triggered samples
                }
                pKey->ReleaseTrigger = false;
            }

            // if neither a voice was spawned or postponed then remove note off event from key again
            if (!pKey->Active && !pKey->VoiceTheftsQueued)
                pKey->pEvents->free(itNoteOffEventOnKeyList);
        }
    }

    /**
     *  Moves pitchbend event from the general (input) event list to the pitch
     *  event list.
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param itPitchbendEvent - absolute pitch value and time stamp of the event
     */
    void Engine::ProcessPitchbend(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itPitchbendEvent) {
        pEngineChannel->Pitch = itPitchbendEvent->Param.Pitch.Pitch; // store current pitch value
        itPitchbendEvent.moveToEndOf(pEngineChannel->pSynthesisEvents[Event::destination_vco]);
    }

    /**
     *  Allocates and triggers a new voice. This method will usually be
     *  called by the ProcessNoteOn() method and by the voices itself
     *  (e.g. to spawn further voices on the same key for layered sounds).
     *
     *  @param pEngineChannel      - engine channel on which this event occured on
     *  @param itNoteOnEvent       - key, velocity and time stamp of the event
     *  @param iLayer              - layer index for the new voice (optional - only
     *                               in case of layered sounds of course)
     *  @param ReleaseTriggerVoice - if new voice is a release triggered voice
     *                               (optional, default = false)
     *  @param VoiceStealing       - if voice stealing should be performed
     *                               when there is no free voice
     *                               (optional, default = true)
     *  @param HandleKeyGroupConflicts - if voices should be killed due to a
     *                                   key group conflict
     *  @returns pointer to new voice or NULL if there was no free voice or
     *           if the voice wasn't triggered (for example when no region is
     *           defined for the given key).
     */
    Pool<Voice>::Iterator Engine::LaunchVoice(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itNoteOnEvent, int iLayer, bool ReleaseTriggerVoice, bool VoiceStealing, bool HandleKeyGroupConflicts) {
        int MIDIKey            = itNoteOnEvent->Param.Note.Key;
        midi_key_info_t* pKey  = &pEngineChannel->pMIDIKeyInfo[MIDIKey];
        ::gig::Region* pRegion = pEngineChannel->pInstrument->GetRegion(MIDIKey);

        // if nothing defined for this key
        if (!pRegion) return Pool<Voice>::Iterator(); // nothing to do

        // only mark the first voice of a layered voice (group) to be in a
        // key group, so the layered voices won't kill each other
        int iKeyGroup = (iLayer == 0 && !ReleaseTriggerVoice) ? pRegion->KeyGroup : 0;

        // handle key group (a.k.a. exclusive group) conflicts
        if (HandleKeyGroupConflicts) {
            if (iKeyGroup) { // if this voice / key belongs to a key group
                uint** ppKeyGroup = &pEngineChannel->ActiveKeyGroups[iKeyGroup];
                if (*ppKeyGroup) { // if there's already an active key in that key group
                    midi_key_info_t* pOtherKey = &pEngineChannel->pMIDIKeyInfo[**ppKeyGroup];
                    // kill all voices on the (other) key
                    RTList<Voice>::Iterator itVoiceToBeKilled = pOtherKey->pActiveVoices->first();
                    RTList<Voice>::Iterator end               = pOtherKey->pActiveVoices->end();
                    for (; itVoiceToBeKilled != end; ++itVoiceToBeKilled) {
                        if (itVoiceToBeKilled->Type != Voice::type_release_trigger) {
                            itVoiceToBeKilled->Kill(itNoteOnEvent);
                            --VoiceSpawnsLeft; //FIXME: just a hack, we should better check in StealVoice() if the voice was killed due to key conflict
                        }
                    }
                }
            }
        }

        Voice::type_t VoiceType = Voice::type_normal;

        // get current dimension values to select the right dimension region
        //TODO: for stolen voices this dimension region selection block is processed twice, this should be changed
        //FIXME: controller values for selecting the dimension region here are currently not sample accurate
        uint DimValues[8] = { 0 };
        for (int i = pRegion->Dimensions - 1; i >= 0; i--) {
            switch (pRegion->pDimensionDefinitions[i].dimension) {
                case ::gig::dimension_samplechannel:
                    DimValues[i] = 0; //TODO: we currently ignore this dimension
                    break;
                case ::gig::dimension_layer:
                    DimValues[i] = iLayer;
                    break;
                case ::gig::dimension_velocity:
                    DimValues[i] = itNoteOnEvent->Param.Note.Velocity;
                    break;
                case ::gig::dimension_channelaftertouch:
                    DimValues[i] = 0; //TODO: we currently ignore this dimension
                    break;
                case ::gig::dimension_releasetrigger:
                    VoiceType = (ReleaseTriggerVoice) ? Voice::type_release_trigger : (!iLayer) ? Voice::type_release_trigger_required : Voice::type_normal;
                    DimValues[i] = (uint) ReleaseTriggerVoice;
                    break;
                case ::gig::dimension_keyboard:
                    DimValues[i] = (uint) pEngineChannel->CurrentKeyDimension;
                    break;
                case ::gig::dimension_roundrobin:
                    DimValues[i] = (uint) pEngineChannel->pMIDIKeyInfo[MIDIKey].RoundRobinIndex; // incremented for each note on
                    break;
                case ::gig::dimension_random:
                    RandomSeed   = RandomSeed * 1103515245 + 12345; // classic pseudo random number generator
                    DimValues[i] = (uint) RandomSeed >> (32 - pRegion->pDimensionDefinitions[i].bits); // highest bits are most random
                    break;
                case ::gig::dimension_modwheel:
                    DimValues[i] = pEngineChannel->ControllerTable[1];
                    break;
                case ::gig::dimension_breath:
                    DimValues[i] = pEngineChannel->ControllerTable[2];
                    break;
                case ::gig::dimension_foot:
                    DimValues[i] = pEngineChannel->ControllerTable[4];
                    break;
                case ::gig::dimension_portamentotime:
                    DimValues[i] = pEngineChannel->ControllerTable[5];
                    break;
                case ::gig::dimension_effect1:
                    DimValues[i] = pEngineChannel->ControllerTable[12];
                    break;
                case ::gig::dimension_effect2:
                    DimValues[i] = pEngineChannel->ControllerTable[13];
                    break;
                case ::gig::dimension_genpurpose1:
                    DimValues[i] = pEngineChannel->ControllerTable[16];
                    break;
                case ::gig::dimension_genpurpose2:
                    DimValues[i] = pEngineChannel->ControllerTable[17];
                    break;
                case ::gig::dimension_genpurpose3:
                    DimValues[i] = pEngineChannel->ControllerTable[18];
                    break;
                case ::gig::dimension_genpurpose4:
                    DimValues[i] = pEngineChannel->ControllerTable[19];
                    break;
                case ::gig::dimension_sustainpedal:
                    DimValues[i] = pEngineChannel->ControllerTable[64];
                    break;
                case ::gig::dimension_portamento:
                    DimValues[i] = pEngineChannel->ControllerTable[65];
                    break;
                case ::gig::dimension_sostenutopedal:
                    DimValues[i] = pEngineChannel->ControllerTable[66];
                    break;
                case ::gig::dimension_softpedal:
                    DimValues[i] = pEngineChannel->ControllerTable[67];
                    break;
                case ::gig::dimension_genpurpose5:
                    DimValues[i] = pEngineChannel->ControllerTable[80];
                    break;
                case ::gig::dimension_genpurpose6:
                    DimValues[i] = pEngineChannel->ControllerTable[81];
                    break;
                case ::gig::dimension_genpurpose7:
                    DimValues[i] = pEngineChannel->ControllerTable[82];
                    break;
                case ::gig::dimension_genpurpose8:
                    DimValues[i] = pEngineChannel->ControllerTable[83];
                    break;
                case ::gig::dimension_effect1depth:
                    DimValues[i] = pEngineChannel->ControllerTable[91];
                    break;
                case ::gig::dimension_effect2depth:
                    DimValues[i] = pEngineChannel->ControllerTable[92];
                    break;
                case ::gig::dimension_effect3depth:
                    DimValues[i] = pEngineChannel->ControllerTable[93];
                    break;
                case ::gig::dimension_effect4depth:
                    DimValues[i] = pEngineChannel->ControllerTable[94];
                    break;
                case ::gig::dimension_effect5depth:
                    DimValues[i] = pEngineChannel->ControllerTable[95];
                    break;
                case ::gig::dimension_none:
                    std::cerr << "gig::Engine::LaunchVoice() Error: dimension=none\n" << std::flush;
                    break;
                default:
                    std::cerr << "gig::Engine::LaunchVoice() Error: Unknown dimension\n" << std::flush;
            }
        }
        ::gig::DimensionRegion* pDimRgn = pRegion->GetDimensionRegionByValue(DimValues);

        // no need to continue if sample is silent
        if (!pDimRgn->pSample || !pDimRgn->pSample->SamplesTotal) return Pool<Voice>::Iterator();

        // allocate a new voice for the key
        Pool<Voice>::Iterator itNewVoice = pKey->pActiveVoices->allocAppend();
        if (itNewVoice) {
            // launch the new voice
            if (itNewVoice->Trigger(pEngineChannel, itNoteOnEvent, pEngineChannel->Pitch, pDimRgn, VoiceType, iKeyGroup) < 0) {
                dmsg(4,("Voice not triggered\n"));
                pKey->pActiveVoices->free(itNewVoice);
            }
            else { // on success
                --VoiceSpawnsLeft;
                if (!pKey->Active) { // mark as active key
                    pKey->Active = true;
                    pKey->itSelf = pEngineChannel->pActiveKeys->allocAppend();
                    *pKey->itSelf = itNoteOnEvent->Param.Note.Key;
                }
                if (itNewVoice->KeyGroup) {
                    uint** ppKeyGroup = &pEngineChannel->ActiveKeyGroups[itNewVoice->KeyGroup];
                    *ppKeyGroup = &*pKey->itSelf; // put key as the (new) active key to its key group
                }
                if (itNewVoice->Type == Voice::type_release_trigger_required) pKey->ReleaseTrigger = true; // mark key for the need of release triggered voice(s)
                return itNewVoice; // success
            }
        }
        else if (VoiceStealing) {
            // try to steal one voice
            int result = StealVoice(pEngineChannel, itNoteOnEvent);
            if (!result) { // voice stolen successfully
                // put note-on event into voice-stealing queue, so it will be reprocessed after killed voice died
                RTList<Event>::Iterator itStealEvent = pVoiceStealingQueue->allocAppend();
                if (itStealEvent) {
                    *itStealEvent = *itNoteOnEvent; // copy event
                    itStealEvent->Param.Note.Layer = iLayer;
                    itStealEvent->Param.Note.ReleaseTrigger = ReleaseTriggerVoice;
                    pKey->VoiceTheftsQueued++;
                }
                else dmsg(1,("Voice stealing queue full!\n"));
            }
        }

        return Pool<Voice>::Iterator(); // no free voice or error
    }

    /**
     *  Will be called by LaunchVoice() method in case there are no free
     *  voices left. This method will select and kill one old voice for
     *  voice stealing and postpone the note-on event until the selected
     *  voice actually died.
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param itNoteOnEvent - key, velocity and time stamp of the event
     *  @returns 0 on success, a value < 0 if no active voice could be picked for voice stealing
     */
    int Engine::StealVoice(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itNoteOnEvent) {
        if (VoiceSpawnsLeft <= 0) {
            dmsg(1,("Max. voice thefts per audio fragment reached (you may raise CONFIG_MAX_VOICES).\n"));
            return -1;
        }
        if (!pEventPool->poolIsEmpty()) {

            RTList<Voice>::Iterator itSelectedVoice;

            // Select one voice for voice stealing
            switch (CONFIG_VOICE_STEAL_ALGO) {

                // try to pick the oldest voice on the key where the new
                // voice should be spawned, if there is no voice on that
                // key, or no voice left to kill, then procceed with
                // 'oldestkey' algorithm
                case voice_steal_algo_oldestvoiceonkey: {
                    midi_key_info_t* pSelectedKey = &pEngineChannel->pMIDIKeyInfo[itNoteOnEvent->Param.Note.Key];
                    itSelectedVoice = pSelectedKey->pActiveVoices->first();
                    // proceed iterating if voice was created in this fragment cycle
                    while (itSelectedVoice && !itSelectedVoice->IsStealable()) ++itSelectedVoice;
                    // if we haven't found a voice then proceed with algorithm 'oldestkey'
                    if (itSelectedVoice && itSelectedVoice->IsStealable()) break;
                } // no break - intentional !

                // try to pick the oldest voice on the oldest active key
                // from the same engine channel
                // (caution: must stay after 'oldestvoiceonkey' algorithm !)
                case voice_steal_algo_oldestkey: {
                    // if we already stole in this fragment, try to proceed on same key
                    if (this->itLastStolenVoice) {
                        itSelectedVoice = this->itLastStolenVoice;
                        do {
                            ++itSelectedVoice;
                        } while (itSelectedVoice && !itSelectedVoice->IsStealable()); // proceed iterating if voice was created in this fragment cycle
                        // found a "stealable" voice ?
                        if (itSelectedVoice && itSelectedVoice->IsStealable()) {
                            // remember which voice we stole, so we can simply proceed on next voice stealing
                            this->itLastStolenVoice = itSelectedVoice;
                            break; // selection succeeded
                        }
                    }
                    // get (next) oldest key
                    RTList<uint>::Iterator iuiSelectedKey = (this->iuiLastStolenKey) ? ++this->iuiLastStolenKey : pEngineChannel->pActiveKeys->first();
                    while (iuiSelectedKey) {
                        midi_key_info_t* pSelectedKey = &pEngineChannel->pMIDIKeyInfo[*iuiSelectedKey];
                        itSelectedVoice = pSelectedKey->pActiveVoices->first();
                        // proceed iterating if voice was created in this fragment cycle
                        while (itSelectedVoice && !itSelectedVoice->IsStealable()) ++itSelectedVoice;
                        // found a "stealable" voice ?
                        if (itSelectedVoice && itSelectedVoice->IsStealable()) {
                            // remember which voice on which key we stole, so we can simply proceed on next voice stealing
                            this->iuiLastStolenKey  = iuiSelectedKey;
                            this->itLastStolenVoice = itSelectedVoice;
                            break; // selection succeeded
                        }
                        ++iuiSelectedKey; // get next oldest key
                    }
                    break;
                }

                // don't steal anything
                case voice_steal_algo_none:
                default: {
                    dmsg(1,("No free voice (voice stealing disabled)!\n"));
                    return -1;
                }
            }

            // if we couldn't steal a voice from the same engine channel then
            // steal oldest voice on the oldest key from any other engine channel
            // (the smaller engine channel number, the higher priority)
            if (!itSelectedVoice || !itSelectedVoice->IsStealable()) {
                EngineChannel* pSelectedChannel;
                int            iChannelIndex;
                // select engine channel
                if (pLastStolenChannel) {
                    pSelectedChannel = pLastStolenChannel;
                    iChannelIndex    = pSelectedChannel->iEngineIndexSelf;
                } else { // pick the engine channel followed by this engine channel
                    iChannelIndex    = (pEngineChannel->iEngineIndexSelf + 1) % engineChannels.size();
                    pSelectedChannel = engineChannels[iChannelIndex];
                }

                // if we already stole in this fragment, try to proceed on same key
                if (this->itLastStolenVoiceGlobally) {
                    itSelectedVoice = this->itLastStolenVoiceGlobally;
                    do {
                        ++itSelectedVoice;
                    } while (itSelectedVoice && !itSelectedVoice->IsStealable()); // proceed iterating if voice was created in this fragment cycle
                }

                #if CONFIG_DEVMODE
                EngineChannel* pBegin = pSelectedChannel; // to detect endless loop
                #endif // CONFIG_DEVMODE

                // did we find a 'stealable' voice?
                if (itSelectedVoice && itSelectedVoice->IsStealable()) {
                    // remember which voice we stole, so we can simply proceed on next voice stealing
                    this->itLastStolenVoiceGlobally = itSelectedVoice;
                } else while (true) { // iterate through engine channels
                    // get (next) oldest key
                    RTList<uint>::Iterator iuiSelectedKey = (this->iuiLastStolenKeyGlobally) ? ++this->iuiLastStolenKeyGlobally : pSelectedChannel->pActiveKeys->first();
                    this->iuiLastStolenKeyGlobally = RTList<uint>::Iterator(); // to prevent endless loop (see line above)
                    while (iuiSelectedKey) {
                        midi_key_info_t* pSelectedKey = &pSelectedChannel->pMIDIKeyInfo[*iuiSelectedKey];
                        itSelectedVoice = pSelectedKey->pActiveVoices->first();
                        // proceed iterating if voice was created in this fragment cycle
                        while (itSelectedVoice && !itSelectedVoice->IsStealable()) ++itSelectedVoice;
                        // found a "stealable" voice ?
                        if (itSelectedVoice && itSelectedVoice->IsStealable()) {
                            // remember which voice on which key on which engine channel we stole, so we can simply proceed on next voice stealing
                            this->iuiLastStolenKeyGlobally  = iuiSelectedKey;
                            this->itLastStolenVoiceGlobally = itSelectedVoice;
                            this->pLastStolenChannel        = pSelectedChannel;
                            goto stealable_voice_found; // selection succeeded
                        }
                        ++iuiSelectedKey; // get next key on current engine channel
                    }
                    // get next engine channel
                    iChannelIndex    = (iChannelIndex + 1) % engineChannels.size();
                    pSelectedChannel = engineChannels[iChannelIndex];

                    #if CONFIG_DEVMODE
                    if (pSelectedChannel == pBegin) {
                        dmsg(1,("FATAL ERROR: voice stealing endless loop!\n"));
                        dmsg(1,("VoiceSpawnsLeft=%d.\n", VoiceSpawnsLeft));
                        dmsg(1,("Exiting.\n"));
                        exit(-1);
                    }
                    #endif // CONFIG_DEVMODE
                }
            }

            // jump point if a 'stealable' voice was found
            stealable_voice_found:

            #if CONFIG_DEVMODE
            if (!itSelectedVoice->IsActive()) {
                dmsg(1,("gig::Engine: ERROR, tried to steal a voice which was not active !!!\n"));
                return -1;
            }
            #endif // CONFIG_DEVMODE

            // now kill the selected voice
            itSelectedVoice->Kill(itNoteOnEvent);

            --VoiceSpawnsLeft;

            return 0; // success
        }
        else {
            dmsg(1,("Event pool emtpy!\n"));
            return -1;
        }
    }

    /**
     *  Removes the given voice from the MIDI key's list of active voices.
     *  This method will be called when a voice went inactive, e.g. because
     *  it finished to playback its sample, finished its release stage or
     *  just was killed.
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param itVoice - points to the voice to be freed
     */
    void Engine::FreeVoice(EngineChannel* pEngineChannel, Pool<Voice>::Iterator& itVoice) {
        if (itVoice) {
            midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[itVoice->MIDIKey];

            uint keygroup = itVoice->KeyGroup;

            // free the voice object
            pVoicePool->free(itVoice);

            // if no other voices left and member of a key group, remove from key group
            if (pKey->pActiveVoices->isEmpty() && keygroup) {
                uint** ppKeyGroup = &pEngineChannel->ActiveKeyGroups[keygroup];
                if (*ppKeyGroup == &*pKey->itSelf) *ppKeyGroup = NULL; // remove key from key group
            }
        }
        else std::cerr << "Couldn't release voice! (!itVoice)\n" << std::flush;
    }

    /**
     *  Called when there's no more voice left on a key, this call will
     *  update the key info respectively.
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param pKey - key which is now inactive
     */
    void Engine::FreeKey(EngineChannel* pEngineChannel, midi_key_info_t* pKey) {
        if (pKey->pActiveVoices->isEmpty()) {
            pKey->Active = false;
            pEngineChannel->pActiveKeys->free(pKey->itSelf); // remove key from list of active keys
            pKey->itSelf = RTList<uint>::Iterator();
            pKey->ReleaseTrigger = false;
            pKey->pEvents->clear();
            dmsg(3,("Key has no more voices now\n"));
        }
        else dmsg(1,("gig::Engine: Oops, tried to free a key which contains voices.\n"));
    }

    /**
     *  Reacts on supported control change commands (e.g. pitch bend wheel,
     *  modulation wheel, aftertouch).
     *
     *  @param pEngineChannel - engine channel on which this event occured on
     *  @param itControlChangeEvent - controller, value and time stamp of the event
     */
    void Engine::ProcessControlChange(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itControlChangeEvent) {
        dmsg(4,("Engine::ContinuousController cc=%d v=%d\n", itControlChangeEvent->Param.CC.Controller, itControlChangeEvent->Param.CC.Value));

        // update controller value in the engine channel's controller table
        pEngineChannel->ControllerTable[itControlChangeEvent->Param.CC.Controller] = itControlChangeEvent->Param.CC.Value;

        // move event from the unsorted event list to the control change event list
        Pool<Event>::Iterator itControlChangeEventOnCCList = itControlChangeEvent.moveToEndOf(pEngineChannel->pCCEvents);

        switch (itControlChangeEventOnCCList->Param.CC.Controller) {
            case 7: { // volume
                //TODO: not sample accurate yet
                pEngineChannel->GlobalVolume = (float) itControlChangeEventOnCCList->Param.CC.Value / 127.0f;
                pEngineChannel->bStatusChanged = true; // engine channel status has changed, so set notify flag
                break;
            }
            case 10: { // panpot
                //TODO: not sample accurate yet
                const int pan = (int) itControlChangeEventOnCCList->Param.CC.Value - 64;
                pEngineChannel->GlobalPanLeft  = 1.0f - float(RTMath::Max(pan, 0)) /  63.0f;
                pEngineChannel->GlobalPanRight = 1.0f - float(RTMath::Min(pan, 0)) / -64.0f;
                break;
            }
            case 64: { // sustain
                if (itControlChangeEventOnCCList->Param.CC.Value >= 64 && !pEngineChannel->SustainPedal) {
                    dmsg(4,("PEDAL DOWN\n"));
                    pEngineChannel->SustainPedal = true;

                    // cancel release process of voices if necessary
                    RTList<uint>::Iterator iuiKey = pEngineChannel->pActiveKeys->first();
                    for (; iuiKey; ++iuiKey) {
                        midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[*iuiKey];
                        if (!pKey->KeyPressed) {
                            RTList<Event>::Iterator itNewEvent = pKey->pEvents->allocAppend();
                            if (itNewEvent) {
                                *itNewEvent = *itControlChangeEventOnCCList; // copy event to the key's own event list
                                itNewEvent->Type = Event::type_cancel_release; // transform event type
                            }
                            else dmsg(1,("Event pool emtpy!\n"));
                        }
                    }
                }
                if (itControlChangeEventOnCCList->Param.CC.Value < 64 && pEngineChannel->SustainPedal) {
                    dmsg(4,("PEDAL UP\n"));
                    pEngineChannel->SustainPedal = false;

                    // release voices if their respective key is not pressed
                    RTList<uint>::Iterator iuiKey = pEngineChannel->pActiveKeys->first();
                    for (; iuiKey; ++iuiKey) {
                        midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[*iuiKey];
                        if (!pKey->KeyPressed) {
                            RTList<Event>::Iterator itNewEvent = pKey->pEvents->allocAppend();
                            if (itNewEvent) {
                                *itNewEvent = *itControlChangeEventOnCCList; // copy event to the key's own event list
                                itNewEvent->Type = Event::type_release; // transform event type
                            }
                            else dmsg(1,("Event pool emtpy!\n"));
                        }
                    }
                }
                break;
            }


            // Channel Mode Messages

            case 120: { // all sound off
                KillAllVoices(pEngineChannel, itControlChangeEventOnCCList);
                break;
            }
            case 121: { // reset all controllers
                pEngineChannel->ResetControllers();
                break;
            }
            case 123: { // all notes off
                ReleaseAllVoices(pEngineChannel, itControlChangeEventOnCCList);
                break;
            }
        }
    }

    /**
     *  Reacts on MIDI system exclusive messages.
     *
     *  @param itSysexEvent - sysex data size and time stamp of the sysex event
     */
    void Engine::ProcessSysex(Pool<Event>::Iterator& itSysexEvent) {
        RingBuffer<uint8_t>::NonVolatileReader reader = pSysexBuffer->get_non_volatile_reader();

        uint8_t exclusive_status, id;
        if (!reader.pop(&exclusive_status)) goto free_sysex_data;
        if (!reader.pop(&id))               goto free_sysex_data;
        if (exclusive_status != 0xF0)       goto free_sysex_data;

        switch (id) {
            case 0x41: { // Roland
                dmsg(3,("Roland Sysex\n"));
                uint8_t device_id, model_id, cmd_id;
                if (!reader.pop(&device_id)) goto free_sysex_data;
                if (!reader.pop(&model_id))  goto free_sysex_data;
                if (!reader.pop(&cmd_id))    goto free_sysex_data;
                if (model_id != 0x42 /*GS*/) goto free_sysex_data;
                if (cmd_id != 0x12 /*DT1*/)  goto free_sysex_data;

                // command address
                uint8_t addr[3]; // 2 byte addr MSB, followed by 1 byte addr LSB)
                const RingBuffer<uint8_t>::NonVolatileReader checksum_reader = reader; // so we can calculate the check sum later
                if (reader.read(&addr[0], 3) != 3) goto free_sysex_data;
                if (addr[0] == 0x40 && addr[1] == 0x00) { // System Parameters
                    dmsg(3,("\tSystem Parameter\n"));
                }
                else if (addr[0] == 0x40 && addr[1] == 0x01) { // Common Parameters
                    dmsg(3,("\tCommon Parameter\n"));
                }
                else if (addr[0] == 0x40 && (addr[1] & 0xf0) == 0x10) { // Part Parameters (1)
                    dmsg(3,("\tPart Parameter\n"));
                    switch (addr[2]) {
                        case 0x40: { // scale tuning
                            dmsg(3,("\t\tScale Tuning\n"));
                            uint8_t scale_tunes[12]; // detuning of all 12 semitones of an octave
                            if (reader.read(&scale_tunes[0], 12) != 12) goto free_sysex_data;
                            uint8_t checksum;
                            if (!reader.pop(&checksum)) goto free_sysex_data;
                            #if CONFIG_ASSERT_GS_SYSEX_CHECKSUM
                            if (GSCheckSum(checksum_reader, 12)) goto free_sysex_data;
                            #endif // CONFIG_ASSERT_GS_SYSEX_CHECKSUM
                            for (int i = 0; i < 12; i++) scale_tunes[i] -= 64;
                            AdjustScale((int8_t*) scale_tunes);
                            dmsg(3,("\t\t\tNew scale applied.\n"));
                            break;
                        }
                    }
                }
                else if (addr[0] == 0x40 && (addr[1] & 0xf0) == 0x20) { // Part Parameters (2)
                }
                else if (addr[0] == 0x41) { // Drum Setup Parameters
                }
                break;
            }
        }

        free_sysex_data: // finally free sysex data
        pSysexBuffer->increment_read_ptr(itSysexEvent->Param.Sysex.Size);
    }

    /**
     * Calculates the Roland GS sysex check sum.
     *
     * @param AddrReader - reader which currently points to the first GS
     *                     command address byte of the GS sysex message in
     *                     question
     * @param DataSize   - size of the GS message data (in bytes)
     */
    uint8_t Engine::GSCheckSum(const RingBuffer<uint8_t>::NonVolatileReader AddrReader, uint DataSize) {
        RingBuffer<uint8_t>::NonVolatileReader reader = AddrReader;
        uint bytes = 3 /*addr*/ + DataSize;
        uint8_t addr_and_data[bytes];
        reader.read(&addr_and_data[0], bytes);
        uint8_t sum = 0;
        for (uint i = 0; i < bytes; i++) sum += addr_and_data[i];
        return 128 - sum % 128;
    }

    /**
     * Allows to tune each of the twelve semitones of an octave.
     *
     * @param ScaleTunes - detuning of all twelve semitones (in cents)
     */
    void Engine::AdjustScale(int8_t ScaleTunes[12]) {
        memcpy(&this->ScaleTuning[0], &ScaleTunes[0], 12); //TODO: currently not sample accurate
    }

    /**
     * Releases all voices on an engine channel. All voices will go into
     * the release stage and thus it might take some time (e.g. dependant to
     * their envelope release time) until they actually die.
     *
     * @param pEngineChannel - engine channel on which all voices should be released
     * @param itReleaseEvent - event which caused this releasing of all voices
     */
    void Engine::ReleaseAllVoices(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itReleaseEvent) {
        RTList<uint>::Iterator iuiKey = pEngineChannel->pActiveKeys->first();
        while (iuiKey) {
            midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[*iuiKey];
            ++iuiKey;
            // append a 'release' event to the key's own event list
            RTList<Event>::Iterator itNewEvent = pKey->pEvents->allocAppend();
            if (itNewEvent) {
                *itNewEvent = *itReleaseEvent; // copy original event (to the key's event list)
                itNewEvent->Type = Event::type_release; // transform event type
            }
            else dmsg(1,("Event pool emtpy!\n"));
        }
    }

    /**
     * Kills all voices on an engine channel as soon as possible. Voices
     * won't get into release state, their volume level will be ramped down
     * as fast as possible.
     *
     * @param pEngineChannel - engine channel on which all voices should be killed
     * @param itKillEvent    - event which caused this killing of all voices
     */
    void Engine::KillAllVoices(EngineChannel* pEngineChannel, Pool<Event>::Iterator& itKillEvent) {
        RTList<uint>::Iterator iuiKey = pEngineChannel->pActiveKeys->first();
        RTList<uint>::Iterator end    = pEngineChannel->pActiveKeys->end();
        while (iuiKey != end) { // iterate through all active keys
            midi_key_info_t* pKey = &pEngineChannel->pMIDIKeyInfo[*iuiKey];
            ++iuiKey;
            RTList<Voice>::Iterator itVoice     = pKey->pActiveVoices->first();
            RTList<Voice>::Iterator itVoicesEnd = pKey->pActiveVoices->end();
            for (; itVoice != itVoicesEnd; ++itVoice) { // iterate through all voices on this key
                itVoice->Kill(itKillEvent);
                --VoiceSpawnsLeft; //FIXME: just a temporary workaround, we should check the cause in StealVoice() instead
            }
        }
    }

    /**
     * Initialize the parameter sequence for the modulation destination given by
     * by 'dst' with the constant value given by val.
     */
    void Engine::ResetSynthesisParameters(Event::destination_t dst, float val) {
        int maxsamples = pAudioOutputDevice->MaxSamplesPerCycle();
        float* m = &pSynthesisParameters[dst][0];
        for (int i = 0; i < maxsamples; i += 4) {
           m[i]   = val;
           m[i+1] = val;
           m[i+2] = val;
           m[i+3] = val;
        }
    }

    uint Engine::VoiceCount() {
        return ActiveVoiceCount;
    }

    uint Engine::VoiceCountMax() {
        return ActiveVoiceCountMax;
    }

    bool Engine::DiskStreamSupported() {
        return true;
    }

    uint Engine::DiskStreamCount() {
        return (pDiskThread) ? pDiskThread->ActiveStreamCount : 0;
    }

    uint Engine::DiskStreamCountMax() {
        return (pDiskThread) ? pDiskThread->ActiveStreamCountMax : 0;
    }

    String Engine::DiskStreamBufferFillBytes() {
        return pDiskThread->GetBufferFillBytes();
    }

    String Engine::DiskStreamBufferFillPercentage() {
        return pDiskThread->GetBufferFillPercentage();
    }

    String Engine::EngineName() {
        return LS_GIG_ENGINE_NAME;
    }

    String Engine::Description() {
        return "Gigasampler Engine";
    }

    String Engine::Version() {
        String s = "$Revision: 1.47 $";
        return s.substr(11, s.size() - 13); // cut dollar signs, spaces and CVS macro keyword
    }

}} // namespace LinuxSampler::gig
