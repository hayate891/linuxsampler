/*
 * Copyright (c) 2014-2017 Christian Schoenebeck
 *
 * http://www.linuxsampler.org
 *
 * This file is part of LinuxSampler and released under the same terms.
 * See README file for details.
 */

#include "InstrumentScriptVMFunctions.h"
#include "InstrumentScriptVM.h"
#include "../AbstractEngineChannel.h"

namespace LinuxSampler {
    
    // play_note() function

    InstrumentScriptVMFunction_play_note::InstrumentScriptVMFunction_play_note(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    VMFnResult* InstrumentScriptVMFunction_play_note::exec(VMFnArgs* args) {
        int note = args->arg(0)->asInt()->evalInt();
        int velocity = (args->argsCount() >= 2) ? args->arg(1)->asInt()->evalInt() : 127;
        int sampleoffset = (args->argsCount() >= 3) ? args->arg(2)->asInt()->evalInt() : 0;
        int duration = (args->argsCount() >= 4) ? args->arg(3)->asInt()->evalInt() : 0; //TODO: -1 might be a better default value instead of 0

        if (note < 0 || note > 127) {
            errMsg("play_note(): argument 1 is an invalid note number");
            return errorResult(0);
        }

        if (velocity < 0 || velocity > 127) {
            errMsg("play_note(): argument 2 is an invalid velocity value");
            return errorResult(0);
        }

        if (sampleoffset < 0) {
            errMsg("play_note(): argument 3 may not be a negative sample offset");
            return errorResult(0);
        } else if (sampleoffset != 0) {
            wrnMsg("play_note(): argument 3 does not support a sample offset other than 0 yet");
        }

        if (duration < -1) {
            errMsg("play_note(): argument 4 must be a duration value of at least -1 or higher");
            return errorResult(0);
        }

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
        e.Init(); // clear IDs
        e.Type = Event::type_play_note;
        e.Param.Note.Key = note;
        e.Param.Note.Velocity = velocity;
        // make this new note dependent to the life time of the original note
        if (duration == -1) {
            if (m_vm->currentVMEventHandler()->eventHandlerType() != VM_EVENT_HANDLER_NOTE) {
                errMsg("play_note(): -1 for argument 4 may only be used for note event handlers");
                return errorResult(0);
            }
            e.Param.Note.ParentNoteID = m_vm->m_event->cause.Param.Note.ID;
        }

        const note_id_t id = pEngineChannel->ScheduleNoteMicroSec(&e, 0);

        // if a duration is supplied (and play-note event was scheduled
        // successfully above), then schedule a subsequent stop-note event
        if (id && duration > 0) {
            e.Type = Event::type_stop_note;
            e.Param.Note.ID = id;
            e.Param.Note.Velocity = 127;
            pEngineChannel->ScheduleEventMicroSec(&e, duration);
        }

        // even if id is null, don't return an errorResult() here, because that
        // would abort the script, and under heavy load it may be considerable
        // that ScheduleNoteMicroSec() fails above, so simply ignore that
        return successResult( ScriptID::fromNoteID(id) );
    }

    // set_controller() function

    InstrumentScriptVMFunction_set_controller::InstrumentScriptVMFunction_set_controller(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    VMFnResult* InstrumentScriptVMFunction_set_controller::exec(VMFnArgs* args) {
        int controller = args->arg(0)->asInt()->evalInt();
        int value      = args->arg(1)->asInt()->evalInt();

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
        e.Init(); // clear IDs
        if (controller == CTRL_TABLE_IDX_AFTERTOUCH) {
            e.Type = Event::type_channel_pressure;
            e.Param.ChannelPressure.Value = value & 127;
        } else if (controller == CTRL_TABLE_IDX_PITCHBEND) {
            e.Type = Event::type_pitchbend;
            e.Param.Pitch.Pitch = value;
        } else if (controller >= 0 && controller <= 127) {
            e.Type = Event::type_control_change;
            e.Param.CC.Controller = controller;
            e.Param.CC.Value = value;
        } else {
            errMsg("set_controller(): argument 1 is an invalid controller");
            return errorResult();
        }

        const event_id_t id = pEngineChannel->ScheduleEventMicroSec(&e, 0);

        // even if id is null, don't return an errorResult() here, because that
        // would abort the script, and under heavy load it may be considerable
        // that ScheduleEventMicroSec() fails above, so simply ignore that
        return successResult( ScriptID::fromEventID(id) );
    }

    // ignore_event() function

    InstrumentScriptVMFunction_ignore_event::InstrumentScriptVMFunction_ignore_event(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_ignore_event::acceptsArgType(int iArg, ExprType_t type) const {
        return type == INT_EXPR || type == INT_ARR_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_ignore_event::exec(VMFnArgs* args) {
        AbstractEngineChannel* pEngineChannel =
                static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("ignore_event(): event ID argument may not be zero");
                // not errorResult(), because that would abort the script, not intentional in this case
                return successResult();
            }
            pEngineChannel->IgnoreEventByScriptID(id);
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);     
                pEngineChannel->IgnoreEventByScriptID(id);
            }
        }

        return successResult();
    }

    // ignore_controller() function

    InstrumentScriptVMFunction_ignore_controller::InstrumentScriptVMFunction_ignore_controller(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    VMFnResult* InstrumentScriptVMFunction_ignore_controller::exec(VMFnArgs* args) {
        const ScriptID id = (args->argsCount() >= 1) ? args->arg(0)->asInt()->evalInt() : m_vm->m_event->id;
        if (!id && args->argsCount() >= 1) {
            wrnMsg("ignore_controller(): event ID argument may not be zero");
            return successResult();
        }

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        pEngineChannel->IgnoreEventByScriptID(id);

        return successResult();
    }

    // note_off() function

    InstrumentScriptVMFunction_note_off::InstrumentScriptVMFunction_note_off(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_note_off::acceptsArgType(int iArg, ExprType_t type) const {
        return type == INT_EXPR || type == INT_ARR_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_note_off::exec(VMFnArgs* args) {
        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        int velocity = (args->argsCount() >= 2) ? args->arg(1)->asInt()->evalInt() : 127;
        if (velocity < 0 || velocity > 127) {
            errMsg("note_off(): argument 2 is an invalid velocity value");
            return errorResult();
        }

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();   
            if (!id) {
                wrnMsg("note_off(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("note_off(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            Event e = pNote->cause;
            e.Init(); // clear IDs
            e.CopyTimeFrom(m_vm->m_event->cause); // set fragment time for "now"
            e.Type = Event::type_stop_note;
            e.Param.Note.ID = id.noteID();
            e.Param.Note.Key = pNote->hostKey;
            e.Param.Note.Velocity = velocity;

            pEngineChannel->ScheduleEventMicroSec(&e, 0);
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                Event e = pNote->cause;
                e.Init(); // clear IDs
                e.CopyTimeFrom(m_vm->m_event->cause); // set fragment time for "now"
                e.Type = Event::type_stop_note;
                e.Param.Note.ID = id.noteID();
                e.Param.Note.Key = pNote->hostKey;
                e.Param.Note.Velocity = velocity;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        }

        return successResult();
    }

    // set_event_mark() function

    InstrumentScriptVMFunction_set_event_mark::InstrumentScriptVMFunction_set_event_mark(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    VMFnResult* InstrumentScriptVMFunction_set_event_mark::exec(VMFnArgs* args) {
        const ScriptID id = args->arg(0)->asInt()->evalInt();
        const int groupID = args->arg(1)->asInt()->evalInt();

        if (groupID < 0 || groupID >= INSTR_SCRIPT_EVENT_GROUPS) {
            errMsg("set_event_mark(): argument 2 is an invalid group id");
            return errorResult();
        }

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        // check if the event/note still exists
        switch (id.type()) {
            case ScriptID::EVENT: {
                RTList<Event>::Iterator itEvent = pEngineChannel->pEngine->EventByID( id.eventID() );
                if (!itEvent) return successResult();
                break;
            }
            case ScriptID::NOTE: {
                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) return successResult();
                break;
            }
        }

        pEngineChannel->pScript->eventGroups[groupID].insert(id);

        return successResult();
    }

    // delete_event_mark() function

    InstrumentScriptVMFunction_delete_event_mark::InstrumentScriptVMFunction_delete_event_mark(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    VMFnResult* InstrumentScriptVMFunction_delete_event_mark::exec(VMFnArgs* args) {
        const ScriptID id = args->arg(0)->asInt()->evalInt();
        const int groupID = args->arg(1)->asInt()->evalInt();

        if (groupID < 0 || groupID >= INSTR_SCRIPT_EVENT_GROUPS) {
            errMsg("delete_event_mark(): argument 2 is an invalid group id");
            return errorResult();
        }

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        pEngineChannel->pScript->eventGroups[groupID].erase(id);

        return successResult();
    }

    // by_marks() function

    InstrumentScriptVMFunction_by_marks::InstrumentScriptVMFunction_by_marks(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    int InstrumentScriptVMFunction_by_marks::Result::arraySize() const {
        return eventGroup->size();
    }

    int InstrumentScriptVMFunction_by_marks::Result::evalIntElement(uint i) {
        return (*eventGroup)[i];
    }

    VMFnResult* InstrumentScriptVMFunction_by_marks::errorResult() {
        m_result.eventGroup = NULL;
        m_result.flags = StmtFlags_t(STMT_ABORT_SIGNALLED | STMT_ERROR_OCCURRED);
        return &m_result;
    }

    VMFnResult* InstrumentScriptVMFunction_by_marks::successResult(EventGroup* eventGroup) {
        m_result.eventGroup = eventGroup;
        m_result.flags = STMT_SUCCESS;
        return &m_result;
    }

    VMFnResult* InstrumentScriptVMFunction_by_marks::exec(VMFnArgs* args) {
        int groupID = args->arg(0)->asInt()->evalInt();

        if (groupID < 0 || groupID >= INSTR_SCRIPT_EVENT_GROUPS) {
            errMsg("by_marks(): argument is an invalid group id");
            return errorResult();
        }

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        return successResult( &pEngineChannel->pScript->eventGroups[groupID] );
    }

    // change_vol() function

    InstrumentScriptVMFunction_change_vol::InstrumentScriptVMFunction_change_vol(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_vol::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_vol::exec(VMFnArgs* args) {
        int volume = args->arg(1)->asInt()->evalInt(); // volume change in milli dB
        bool relative = (args->argsCount() >= 3) ? (args->arg(2)->asInt()->evalInt() & 1) : false;
        const float fVolumeLin = RTMath::DecibelToLinRatio(float(volume) / 1000.f);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_vol(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_vol(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_vol() was called immediately after note was triggered
            // then immediately apply the volume to note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                if (relative)
                    pNote->Override.Volume *= fVolumeLin;
                else
                    pNote->Override.Volume = fVolumeLin;
            } else { // otherwise schedule the volume change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_volume;
                e.Param.NoteSynthParam.Delta    = fVolumeLin;
                e.Param.NoteSynthParam.Relative = relative;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_vol() was called immediately after note was triggered
                // then immediately apply the volume to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    if (relative)
                        pNote->Override.Volume *= fVolumeLin;
                    else
                        pNote->Override.Volume = fVolumeLin;
                } else { // otherwise schedule the volume change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_volume;
                    e.Param.NoteSynthParam.Delta    = fVolumeLin;
                    e.Param.NoteSynthParam.Relative = relative;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    // change_tune() function

    InstrumentScriptVMFunction_change_tune::InstrumentScriptVMFunction_change_tune(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_tune::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_tune::exec(VMFnArgs* args) {
        int tune = args->arg(1)->asInt()->evalInt(); // tuning change in milli cents
        bool relative = (args->argsCount() >= 3) ? (args->arg(2)->asInt()->evalInt() & 1) : false;
        const float fFreqRatio = RTMath::CentsToFreqRatioUnlimited(float(tune) / 1000.f);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_tune(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_tune(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_tune() was called immediately after note was triggered
            // then immediately apply the tuning to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                if (relative) 
                    pNote->Override.Pitch *= fFreqRatio;
                else
                    pNote->Override.Pitch = fFreqRatio;
            } else { // otherwise schedule tuning change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_pitch;
                e.Param.NoteSynthParam.Delta    = fFreqRatio;
                e.Param.NoteSynthParam.Relative = relative;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_tune() was called immediately after note was triggered
                // then immediately apply the tuning to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    if (relative) 
                        pNote->Override.Pitch *= fFreqRatio;
                    else
                        pNote->Override.Pitch = fFreqRatio;
                } else { // otherwise schedule tuning change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_pitch;
                    e.Param.NoteSynthParam.Delta    = fFreqRatio;
                    e.Param.NoteSynthParam.Relative = relative;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    // change_pan() function

    InstrumentScriptVMFunction_change_pan::InstrumentScriptVMFunction_change_pan(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_pan::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_pan::exec(VMFnArgs* args) {
        int pan = args->arg(1)->asInt()->evalInt();
        bool relative = (args->argsCount() >= 3) ? (args->arg(2)->asInt()->evalInt() & 1) : false;

        if (pan > 1000) {
            wrnMsg("change_pan(): argument 2 may not be larger than 1000");
            pan = 1000;
        } else if (pan < -1000) {
            wrnMsg("change_pan(): argument 2 may not be smaller than -1000");
            pan = -1000;
        }
        const float fPan = float(pan) / 1000.f;

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_pan(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_pan(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_pan() was called immediately after note was triggered
            // then immediately apply the panning to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                if (relative) {
                    pNote->Override.Pan = RTMath::RelativeSummedAvg(pNote->Override.Pan, fPan, ++pNote->Override.PanSources);
                } else {
                    pNote->Override.Pan = fPan;
                    pNote->Override.PanSources = 1; // only relevant on subsequent change_pan() calls on same note with 'relative' being set
                }
            } else { // otherwise schedule panning change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_pan;
                e.Param.NoteSynthParam.Delta    = fPan;
                e.Param.NoteSynthParam.Relative = relative;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_pan() was called immediately after note was triggered
                // then immediately apply the panning to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    if (relative) {
                        pNote->Override.Pan = RTMath::RelativeSummedAvg(pNote->Override.Pan, fPan, ++pNote->Override.PanSources);
                    } else {
                        pNote->Override.Pan = fPan;
                        pNote->Override.PanSources = 1; // only relevant on subsequent change_pan() calls on same note with 'relative' being set
                    }
                } else { // otherwise schedule panning change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_pan;
                    e.Param.NoteSynthParam.Delta    = fPan;
                    e.Param.NoteSynthParam.Relative = relative;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    #define VM_FILTER_PAR_MAX_VALUE 1000000
    #define VM_EG_PAR_MAX_VALUE 1000000

    // change_cutoff() function

    InstrumentScriptVMFunction_change_cutoff::InstrumentScriptVMFunction_change_cutoff(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_cutoff::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_cutoff::exec(VMFnArgs* args) {
        int cutoff = args->arg(1)->asInt()->evalInt();
        if (cutoff > VM_FILTER_PAR_MAX_VALUE) {
            wrnMsg("change_cutoff(): argument 2 may not be larger than 1000000");
            cutoff = VM_FILTER_PAR_MAX_VALUE;
        } else if (cutoff < 0) {
            wrnMsg("change_cutoff(): argument 2 may not be negative");
            cutoff = 0;
        }
        const float fCutoff = float(cutoff) / float(VM_FILTER_PAR_MAX_VALUE);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_cutoff(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_cutoff(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_cutoff() was called immediately after note was triggered
            // then immediately apply cutoff to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                pNote->Override.Cutoff = fCutoff;
            } else { // otherwise schedule cutoff change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_cutoff;
                e.Param.NoteSynthParam.Delta    = fCutoff;
                e.Param.NoteSynthParam.Relative = false;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_cutoff() was called immediately after note was triggered
                // then immediately apply cutoff to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    pNote->Override.Cutoff = fCutoff;
                } else { // otherwise schedule cutoff change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_cutoff;
                    e.Param.NoteSynthParam.Delta    = fCutoff;
                    e.Param.NoteSynthParam.Relative = false;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    // change_reso() function
    
    InstrumentScriptVMFunction_change_reso::InstrumentScriptVMFunction_change_reso(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_reso::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_reso::exec(VMFnArgs* args) {
        int resonance = args->arg(1)->asInt()->evalInt();
        if (resonance > VM_FILTER_PAR_MAX_VALUE) {
            wrnMsg("change_reso(): argument 2 may not be larger than 1000000");
            resonance = VM_FILTER_PAR_MAX_VALUE;
        } else if (resonance < 0) {
            wrnMsg("change_reso(): argument 2 may not be negative");
            resonance = 0;
        }
        const float fResonance = float(resonance) / float(VM_FILTER_PAR_MAX_VALUE);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_reso(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_reso(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_reso() was called immediately after note was triggered
            // then immediately apply resonance to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                pNote->Override.Resonance = fResonance;
            } else { // otherwise schedule resonance change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_resonance;
                e.Param.NoteSynthParam.Delta    = fResonance;
                e.Param.NoteSynthParam.Relative = false;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_reso() was called immediately after note was triggered
                // then immediately apply resonance to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    pNote->Override.Resonance = fResonance;
                } else { // otherwise schedule resonance change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_resonance;
                    e.Param.NoteSynthParam.Delta    = fResonance;
                    e.Param.NoteSynthParam.Relative = false;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }
    
    // change_attack() function

    InstrumentScriptVMFunction_change_attack::InstrumentScriptVMFunction_change_attack(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_attack::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_attack::exec(VMFnArgs* args) {
        int attack = args->arg(1)->asInt()->evalInt();
        if (attack > VM_EG_PAR_MAX_VALUE) {
            wrnMsg("change_attack(): argument 2 may not be larger than 1000000");
            attack = VM_EG_PAR_MAX_VALUE;
        } else if (attack < 0) {
            wrnMsg("change_attack(): argument 2 may not be negative");
            attack = 0;
        }
        const float fAttack = float(attack) / float(VM_EG_PAR_MAX_VALUE);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_attack(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_attack(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_attack() was called immediately after note was triggered
            // then immediately apply attack to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                pNote->Override.Attack = fAttack;
            } else { // otherwise schedule attack change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_attack;
                e.Param.NoteSynthParam.Delta    = fAttack;
                e.Param.NoteSynthParam.Relative = false;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_attack() was called immediately after note was triggered
                // then immediately apply attack to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    pNote->Override.Attack = fAttack;
                } else { // otherwise schedule attack change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_attack;
                    e.Param.NoteSynthParam.Delta    = fAttack;
                    e.Param.NoteSynthParam.Relative = false;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    // change_decay() function
    
    InstrumentScriptVMFunction_change_decay::InstrumentScriptVMFunction_change_decay(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_decay::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_decay::exec(VMFnArgs* args) {
        int decay = args->arg(1)->asInt()->evalInt();
        if (decay > VM_EG_PAR_MAX_VALUE) {
            wrnMsg("change_decay(): argument 2 may not be larger than 1000000");
            decay = VM_EG_PAR_MAX_VALUE;
        } else if (decay < 0) {
            wrnMsg("change_decay(): argument 2 may not be negative");
            decay = 0;
        }
        const float fDecay = float(decay) / float(VM_EG_PAR_MAX_VALUE);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_decay(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_decay(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_decay() was called immediately after note was triggered
            // then immediately apply decay to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                pNote->Override.Decay = fDecay;
            } else { // otherwise schedule decay change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_decay;
                e.Param.NoteSynthParam.Delta    = fDecay;
                e.Param.NoteSynthParam.Relative = false;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_decay() was called immediately after note was triggered
                // then immediately apply decay to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    pNote->Override.Decay = fDecay;
                } else { // otherwise schedule decay change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_decay;
                    e.Param.NoteSynthParam.Delta    = fDecay;
                    e.Param.NoteSynthParam.Relative = false;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    // change_release() function
    
    InstrumentScriptVMFunction_change_release::InstrumentScriptVMFunction_change_release(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    bool InstrumentScriptVMFunction_change_release::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    VMFnResult* InstrumentScriptVMFunction_change_release::exec(VMFnArgs* args) {
        int release = args->arg(1)->asInt()->evalInt();
        if (release > VM_EG_PAR_MAX_VALUE) {
            wrnMsg("change_release(): argument 2 may not be larger than 1000000");
            release = VM_EG_PAR_MAX_VALUE;
        } else if (release < 0) {
            wrnMsg("change_release(): argument 2 may not be negative");
            release = 0;
        }
        const float fRelease = float(release) / float(VM_EG_PAR_MAX_VALUE);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg("change_release(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg("change_release(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if change_release() was called immediately after note was triggered
            // then immediately apply relase to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                pNote->Override.Release = fRelease;
            } else { // otherwise schedule release change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = Event::synth_param_release;
                e.Param.NoteSynthParam.Delta    = fRelease;
                e.Param.NoteSynthParam.Relative = false;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if change_release() was called immediately after note was triggered
                // then immediately apply relase to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    pNote->Override.Release = fRelease;
                } else { // otherwise schedule release change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = Event::synth_param_release;
                    e.Param.NoteSynthParam.Delta    = fRelease;
                    e.Param.NoteSynthParam.Relative = false;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    #define VM_GENERAL_CHANGE_SYNTH_PAR_MAX_VALUE 1000000

    bool VMChangeSynthParamFunction::acceptsArgType(int iArg, ExprType_t type) const {
        if (iArg == 0)
            return type == INT_EXPR || type == INT_ARR_EXPR;
        else
            return INT_EXPR;
    }

    template<float NoteBase::_Override::*T_noteParam, int T_synthParam>
    VMFnResult* VMChangeSynthParamFunction::execTemplate(VMFnArgs* args, const char* functionName) {
        int value = args->arg(1)->asInt()->evalInt();
        if (value > VM_GENERAL_CHANGE_SYNTH_PAR_MAX_VALUE) {
            wrnMsg(String(functionName) + "(): argument 2 may not be larger than 1000000");
            value = VM_GENERAL_CHANGE_SYNTH_PAR_MAX_VALUE;
        } else if (value < 0) {
            wrnMsg(String(functionName) + "(): argument 2 may not be negative");
            value = 0;
        }
        const float fValue = float(value) / float(VM_GENERAL_CHANGE_SYNTH_PAR_MAX_VALUE);

        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        if (args->arg(0)->exprType() == INT_EXPR) {
            const ScriptID id = args->arg(0)->asInt()->evalInt();
            if (!id) {
                wrnMsg(String(functionName) + "(): note ID for argument 1 may not be zero");
                return successResult();
            }
            if (!id.isNoteID()) {
                wrnMsg(String(functionName) + "(): argument 1 is not a note ID");
                return successResult();
            }

            NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
            if (!pNote) return successResult();

            // if this change_*() script function was called immediately after
            // note was triggered then immediately apply the synth parameter
            // change to Note object
            if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                pNote->Override.*T_noteParam = fValue;
            } else { // otherwise schedule this synth parameter change ...
                Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                e.Init(); // clear IDs
                e.Type = Event::type_note_synth_param;
                e.Param.NoteSynthParam.NoteID   = id.noteID();
                e.Param.NoteSynthParam.Type     = (Event::synth_param_t) T_synthParam;
                e.Param.NoteSynthParam.Delta    = fValue;
                e.Param.NoteSynthParam.Relative = false;

                pEngineChannel->ScheduleEventMicroSec(&e, 0);
            }
        } else if (args->arg(0)->exprType() == INT_ARR_EXPR) {
            VMIntArrayExpr* ids = args->arg(0)->asIntArray();
            for (int i = 0; i < ids->arraySize(); ++i) {
                const ScriptID id = ids->evalIntElement(i);
                if (!id || !id.isNoteID()) continue;

                NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
                if (!pNote) continue;

                // if this change_*() script function was called immediately after
                // note was triggered then immediately apply the synth parameter
                // change to Note object
                if (m_vm->m_event->cause.SchedTime() == pNote->triggerSchedTime) {
                    pNote->Override.*T_noteParam = fValue;
                } else { // otherwise schedule this synth parameter change ...
                    Event e = m_vm->m_event->cause; // copy to get fragment time for "now"
                    e.Init(); // clear IDs
                    e.Type = Event::type_note_synth_param;
                    e.Param.NoteSynthParam.NoteID   = id.noteID();
                    e.Param.NoteSynthParam.Type     = (Event::synth_param_t) T_synthParam;
                    e.Param.NoteSynthParam.Delta    = fValue;
                    e.Param.NoteSynthParam.Relative = false;

                    pEngineChannel->ScheduleEventMicroSec(&e, 0);
                }
            }
        }

        return successResult();
    }

    // change_amp_lfo_depth() function

    VMFnResult* InstrumentScriptVMFunction_change_amp_lfo_depth::exec(VMFnArgs* args) {
        return VMChangeSynthParamFunction::execTemplate< &NoteBase::_Override::AmpLFODepth, Event::synth_param_amp_lfo_depth >(args, "change_amp_lfo_depth");
    }

    // change_amp_lfo_freq() function

    VMFnResult* InstrumentScriptVMFunction_change_amp_lfo_freq::exec(VMFnArgs* args) {
        return VMChangeSynthParamFunction::execTemplate< &NoteBase::_Override::AmpLFOFreq, Event::synth_param_amp_lfo_freq >(args, "change_amp_lfo_freq");
    }

    // change_pitch_lfo_depth() function

    VMFnResult* InstrumentScriptVMFunction_change_pitch_lfo_depth::exec(VMFnArgs* args) {
        return VMChangeSynthParamFunction::execTemplate< &NoteBase::_Override::PitchLFODepth, Event::synth_param_pitch_lfo_depth >(args, "change_pitch_lfo_depth");
    }

    // change_pitch_lfo_freq() function

    VMFnResult* InstrumentScriptVMFunction_change_pitch_lfo_freq::exec(VMFnArgs* args) {
        return VMChangeSynthParamFunction::execTemplate< &NoteBase::_Override::PitchLFOFreq, Event::synth_param_pitch_lfo_freq >(args, "change_pitch_lfo_freq");
    }

    // event_status() function

    InstrumentScriptVMFunction_event_status::InstrumentScriptVMFunction_event_status(InstrumentScriptVM* parent)
        : m_vm(parent)
    {
    }

    VMFnResult* InstrumentScriptVMFunction_event_status::exec(VMFnArgs* args) {
        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        const ScriptID id = args->arg(0)->asInt()->evalInt();
        if (!id) {
            wrnMsg("event_status(): note ID for argument 1 may not be zero");
            return successResult(EVENT_STATUS_INACTIVE);
        }
        if (!id.isNoteID()) {
            wrnMsg("event_status(): argument 1 is not a note ID");
            return successResult(EVENT_STATUS_INACTIVE);
        }

        NoteBase* pNote = pEngineChannel->pEngine->NoteByID( id.noteID() );
        return successResult(pNote ? EVENT_STATUS_NOTE_QUEUE : EVENT_STATUS_INACTIVE);
    }

    // wait() function (overrides core wait() implementation)

    InstrumentScriptVMFunction_wait::InstrumentScriptVMFunction_wait(InstrumentScriptVM* parent)
        : CoreVMFunction_wait(parent)
    {    
    }

    VMFnResult* InstrumentScriptVMFunction_wait::exec(VMFnArgs* args) {
        InstrumentScriptVM* m_vm = (InstrumentScriptVM*) vm;

        // this might be set by passing 1 with the 2nd argument of built-in stop_wait() function
        if (m_vm->m_event->ignoreAllWaitCalls) return successResult();

        return CoreVMFunction_wait::exec(args);
    }

    // stop_wait() function

    InstrumentScriptVMFunction_stop_wait::InstrumentScriptVMFunction_stop_wait(InstrumentScriptVM* parent)
        : m_vm(parent)
    {    
    }

    VMFnResult* InstrumentScriptVMFunction_stop_wait::exec(VMFnArgs* args) {
        AbstractEngineChannel* pEngineChannel =
            static_cast<AbstractEngineChannel*>(m_vm->m_event->cause.pEngineChannel);

        const script_callback_id_t id = args->arg(0)->asInt()->evalInt();
        if (!id) {
            wrnMsg("stop_wait(): callback ID for argument 1 may not be zero");
            return successResult();
        }

        RTList<ScriptEvent>::Iterator itCallback = pEngineChannel->ScriptCallbackByID(id);
        if (!itCallback) return successResult(); // ignore if callback is i.e. not alive anymore

        const bool disableWaitForever =
            (args->argsCount() >= 2) ? (args->arg(1)->asInt()->evalInt() == 1) : false;

        pEngineChannel->ScheduleResumeOfScriptCallback(
            itCallback, m_vm->m_event->cause.SchedTime(), disableWaitForever
        );

        return successResult();
    }

} // namespace LinuxSampler
