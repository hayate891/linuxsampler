/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2005 Christian Schoenebeck                              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this library; if not, write to the Free Software           *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,                 *
 *   MA  02111-1307  USA                                                   *
 ***************************************************************************/

#ifndef __LSCPINSTRUMENTLOADER_H__
#define __LSCPINSTRUMENTLOADER_H__

#include "../common/global.h"
#include "../common/Thread.h"
#include "../common/RingBuffer.h"
#include "../Sampler.h"

/// Maximum numbers of instruments waiting to be loaded by the LSCPInstrumentLoader.
#define INSTRUMENT_LOADER_QUEUE_SIZE    200

using namespace LinuxSampler;

/**
 * Instrument loader thread for the LinuxSampler Control Protocol (LSCP)
 * Server for loading instruments in the background. This loader will only
 * load one instrument by one.
 */
class LSCPInstrumentLoader : public Thread {
    public:
        LSCPInstrumentLoader();
        void StartNewLoad(String Filename, uint uiInstrumentIndex, Engine* pEngine);
        virtual ~LSCPInstrumentLoader();
    protected:
        struct command_t {
            String  Filename;
            uint    uiInstrumentIndex;
            Engine* pEngine;
        };

        // Instance variables.
        RingBuffer<command_t>* pQueue; ///< queue with commands for loading new instruments.

        int Main(); ///< Implementation of virtual method from class Thread.
};

#endif // __LSCPINSTRUMENTLOADER_H__
