// qsamplerChannel.cpp
//
/****************************************************************************
   Copyright (C) 2003-2004, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*****************************************************************************/

#include "qsamplerChannel.h"

#include "qsamplerMainForm.h"
#include "qsamplerChannelForm.h"

#include "config.h"

#include <qfileinfo.h>

#ifdef CONFIG_LIBGIG
#include "gig.h"
#else
#define QSAMPLER_INSTRUMENT_MAX 8
#endif


//-------------------------------------------------------------------------
// qsamplerChannel - Sampler channel structure.
//

// Constructor.
qsamplerChannel::qsamplerChannel ( qsamplerMainForm *pMainForm, int iChannelID )
{
    m_pMainForm  = pMainForm;
    m_iChannelID = iChannelID;

//  m_sEngineName       = QObject::tr("(No engine)");
//  m_sInstrumentFile   = QObject::tr("(No instrument)");
    m_iInstrumentNr     = -1;
    m_iInstrumentStatus = -1;
    m_sMidiDriver       = "Alsa";   // DEPRECATED.
    m_iMidiDevice       = -1;
    m_iMidiPort         = -1;
    m_iMidiChannel      = -1;
    m_sAudioDriver      = "Alsa";   // DEPRECATED.
    m_iAudioDevice      = -1;
    m_fVolume           = 0.0;

}

// Default destructor.
qsamplerChannel::~qsamplerChannel (void)
{
}


// The global options settings delegated property.
qsamplerOptions *qsamplerChannel::options (void)
{
    if (m_pMainForm == NULL)
        return NULL;

    return m_pMainForm->options();
}


// The client descriptor delegated property.
lscp_client_t *qsamplerChannel::client (void)
{
    if (m_pMainForm == NULL)
        return NULL;

    return m_pMainForm->client();
}


// Create a new sampler channel, if not already.
bool qsamplerChannel::addChannel (void)
{
    if (client() == NULL)
        return false;

    // Are we a new channel?
    if (m_iChannelID < 0) {
        m_iChannelID = ::lscp_add_channel(client());
        if (m_iChannelID < 0) {
            appendMessagesClient("lscp_add_channel");
            appendMessagesError(QObject::tr("Could not create the new channel.\n\nSorry."));
        }   // Otherwise it's created...
        else appendMessages(QObject::tr("Channel %1 created.").arg(m_iChannelID));
    }

    // Return whether we're a valid channel...
    return (m_iChannelID >= 0);
}


// Remove sampler channel.
bool qsamplerChannel::removeChannel (void)
{
    if (client() == NULL)
        return false;

    // Are we an existing channel?
    if (m_iChannelID >= 0) {
        if (::lscp_remove_channel(client(), m_iChannelID) != LSCP_OK) {
            appendMessagesClient("lscp_remove_channel");
            appendMessagesError(QObject::tr("Could not remove channel.\n\nSorry."));
        } else {
            // Otherwise it's removed.
            appendMessages(QObject::tr("Channel %1 removed.").arg(m_iChannelID));
            m_iChannelID = -1;
        }
    }
    
    // Return whether we've removed the channel...
    return (m_iChannelID < 0);
}


// Channel-ID (aka Sammpler-Channel) accessors.
int qsamplerChannel::channelID (void)
{
    return m_iChannelID;
}

void qsamplerChannel::setChannelID ( int iChannelID )
{
    m_iChannelID = iChannelID;
}


// Readable channel name.
QString qsamplerChannel::channelName (void)
{
    return (m_iChannelID < 0 ? QObject::tr("New Channel") : QObject::tr("Channel %1").arg(m_iChannelID));
}


// Engine name accessors.
QString& qsamplerChannel::engineName (void)
{
    return m_sEngineName;
}

bool qsamplerChannel::loadEngine ( const QString& sEngineName )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_load_engine(client(), sEngineName.latin1(), m_iChannelID) != LSCP_OK) {
        appendMessagesClient("lscp_load_engine");
        return false;
    }

    m_sEngineName = sEngineName;
    return true;
}


// Instrument filename accessor.
QString& qsamplerChannel::instrumentFile (void)
{
    return m_sInstrumentFile;
}

// Instrument index accessor.
int qsamplerChannel::instrumentNr (void)
{
    return m_iInstrumentNr;
}

// Instrument status accessor.
int qsamplerChannel::instrumentStatus (void)
{
    return m_iInstrumentStatus;
}

// Instrument file loader.
bool qsamplerChannel::loadInstrument ( const QString& sInstrumentFile, int iInstrumentNr )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_load_instrument_non_modal(client(), sInstrumentFile.latin1(), iInstrumentNr, m_iChannelID) != LSCP_OK) {
        appendMessagesClient("lscp_load_instrument");
        return false;
    }

    m_sInstrumentFile = sInstrumentFile;
    m_iInstrumentNr = iInstrumentNr;
    m_iInstrumentStatus = 0;

    return true;
}


// MIDI driver type accessors (DEPRECATED).
QString& qsamplerChannel::midiDriver (void)
{
    return m_sMidiDriver;
}

bool qsamplerChannel::setMidiDriver ( const QString& sMidiDriver )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_midi_type(client(), m_iChannelID, sMidiDriver.latin1()) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_midi_type");
        return false;
    }

    m_sMidiDriver = sMidiDriver;
    return true;
}


// MIDI device accessors.
int qsamplerChannel::midiDevice (void)
{
    return m_iMidiDevice;
}

bool qsamplerChannel::setMidiDevice ( int iMidiDevice )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_midi_device(client(), m_iChannelID, iMidiDevice) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_midi_device");
        return false;
    }

    m_iMidiDevice = iMidiDevice;
    return true;
}


// MIDI port number accessor.
int qsamplerChannel::midiPort (void)
{
    return m_iMidiPort;
}

bool qsamplerChannel::setMidiPort ( int iMidiPort )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_midi_port(client(), m_iChannelID, iMidiPort) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_midi_port");
        return false;
    }

    m_iMidiPort = iMidiPort;
    return true;
}


// MIDI channel accessor.
int qsamplerChannel::midiChannel (void)
{
    return m_iMidiChannel;
}

bool qsamplerChannel::setMidiChannel ( int iMidiChannel )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_midi_channel(client(), m_iChannelID, iMidiChannel) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_midi_channel");
        return false;
    }

    m_iMidiChannel = iMidiChannel;
    return true;
}


// Audio device accessor.
int qsamplerChannel::audioDevice (void)
{
    return m_iAudioDevice;
}

bool qsamplerChannel::setAudioDevice ( int iAudioDevice )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_audio_device(client(), m_iChannelID, iAudioDevice) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_audio_device");
        return false;
    }

    m_iAudioDevice = iAudioDevice;
    return true;
}


// Audio driver type accessors (DEPRECATED).
QString& qsamplerChannel::audioDriver (void)
{
    return m_sAudioDriver;
}

bool qsamplerChannel::setAudioDriver ( const QString& sAudioDriver )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_audio_type(client(), m_iChannelID, sAudioDriver.latin1()) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_audio_type");
        return false;
    }

    m_sAudioDriver = sAudioDriver;
    return true;
}


// Channel volume accessors.
float qsamplerChannel::volume (void)
{
    return m_fVolume;
}

bool qsamplerChannel::setVolume ( float fVolume )
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_set_channel_volume(client(), m_iChannelID, fVolume) != LSCP_OK) {
        appendMessagesClient("lscp_set_channel_volume");
        return false;
    }

    m_fVolume = fVolume;
    return true;
}


// Update whole channel info state.
bool qsamplerChannel::updateChannelInfo (void)
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    // Read channel information.
    lscp_channel_info_t *pChannelInfo = ::lscp_get_channel_info(client(), m_iChannelID);
    if (pChannelInfo == NULL) {
        appendMessagesClient("lscp_get_channel_info");
        appendMessagesError(QObject::tr("Could not get channel information.\n\nSorry."));
        return false;
    }

    // Cache in channel information.
    m_sEngineName       = pChannelInfo->engine_name;
    m_sInstrumentFile   = pChannelInfo->instrument_file;
    m_iInstrumentNr     = pChannelInfo->instrument_nr;
    m_iInstrumentStatus = pChannelInfo->instrument_status;
    m_iMidiDevice       = pChannelInfo->midi_device;
    m_iMidiPort         = pChannelInfo->midi_port;
    m_iMidiChannel      = pChannelInfo->midi_channel;
    m_iAudioDevice      = pChannelInfo->audio_device;
    m_fVolume           = pChannelInfo->volume;
    // Some sanity checks.
    if (m_sEngineName == "NONE")
        m_sEngineName = QString::null;
    if (m_sInstrumentFile == "NONE")
        m_sInstrumentFile = QString::null;

    return true;
}


// Reset channel method.
bool qsamplerChannel::resetChannel (void)
{
    if (client() == NULL || m_iChannelID < 0)
        return false;

    if (::lscp_reset_channel(client(), m_iChannelID) != LSCP_OK) {
        appendMessagesClient("lscp_reset_channel");
        return false;
    }

    appendMessages(QObject::tr("Channel %1 reset.").arg(m_iChannelID));
    return true;
}


// Channel setup dialog form.
bool qsamplerChannel::channelSetup ( QWidget *pParent )
{
    bool bResult = false;

    qsamplerChannelForm *pChannelForm = new qsamplerChannelForm(pParent);
    if (pChannelForm) {
        pChannelForm->setup(this);
        bResult = pChannelForm->exec();
        delete pChannelForm;
    }

    return bResult;
}


// Redirected messages output methods.
void qsamplerChannel::appendMessages( const QString& s )
{
    if (m_pMainForm) m_pMainForm->appendMessages(s);
}

void qsamplerChannel::appendMessagesColor( const QString& s, const QString& c )
{
    if (m_pMainForm) m_pMainForm->appendMessagesColor(s, c);
}

void qsamplerChannel::appendMessagesText( const QString& s )
{
    if (m_pMainForm) m_pMainForm->appendMessagesText(s);
}

void qsamplerChannel::appendMessagesError( const QString& s )
{
    if (m_pMainForm) m_pMainForm->appendMessagesError(s);
}

void qsamplerChannel::appendMessagesClient( const QString& s )
{
    if (m_pMainForm) m_pMainForm->appendMessagesClient(s);
}


// Context menu event handler.
void qsamplerChannel::contextMenuEvent( QContextMenuEvent *pEvent )
{
    if (m_pMainForm) m_pMainForm->contextMenuEvent(pEvent);
}


// Retrieve the instrument list of a instrument file (.gig).
QStringList qsamplerChannel::getInstrumentList( const QString& sInstrumentFile )
{
    QFileInfo fileinfo(sInstrumentFile);
    QString sInstrumentName = fileinfo.fileName();
    QStringList instlist;

    if (fileinfo.exists()) {
#ifdef CONFIG_LIBGIG
        RIFF::File *pRiff = new RIFF::File(sInstrumentFile);
        gig::File  *pGig  = new gig::File(pRiff);
        gig::Instrument *pInstrument = pGig->GetFirstInstrument();
        while (pInstrument) {
            instlist.append((pInstrument->pInfo)->Name.c_str());
            pInstrument = pGig->GetNextInstrument();
        }
        delete pGig;
        delete pRiff;
#else
        for (int iInstrumentNr = 0; iInstrumentNr < QSAMPLER_INSTRUMENT_MAX; iInstrumentNr++)
            instlist.append(sInstrumentName + " [" + QString::number(iInstrumentNr) + "]");
#endif
    }
    else instlist.append(sInstrumentName);

    return instlist;
}


// Retrieve the spacific instrument name of a instrument file (.gig), given its index.
QString qsamplerChannel::getInstrumentName( const QString& sInstrumentFile, int iInstrumentNr )
{
    QFileInfo fileinfo(sInstrumentFile);
    QString sInstrumentName = fileinfo.fileName();

    if (fileinfo.exists()) {
#ifdef CONFIG_LIBGIG
        RIFF::File *pRiff = new RIFF::File(sInstrumentFile);
        gig::File  *pGig  = new gig::File(pRiff);
        int iIndex = 0;
        gig::Instrument *pInstrument = pGig->GetFirstInstrument();
        while (pInstrument) {
            if (iIndex == iInstrumentNr) {
                sInstrumentName = (pInstrument->pInfo)->Name.c_str();
                break;
            }
            iIndex++;
            pInstrument = pGig->GetNextInstrument();
        }
        delete pGig;
        delete pRiff;
#else
        sInstrumentName += " [" + QString::number(iInstrumentNr) + "]";
#endif
    }

    return sInstrumentName;
}


// end of qsamplerChannel.cpp
