/*
 *   JSampler - a java front-end for LinuxSampler
 *
 *   Copyright (C) 2005 Grigor Kirilov Iliev
 *
 *   This file is part of JSampler.
 *
 *   JSampler is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2
 *   as published by the Free Software Foundation.
 *
 *   JSampler is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with JSampler; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *   MA  02111-1307  USA
 */

package org.jsampler.task;

import java.util.logging.Level;

import org.jsampler.CC;
import org.jsampler.HF;

import static org.jsampler.JSI18n.i18n;


/**
 * This task sets the audio output device of a specific sampler channel.
 * @author Grigor Iliev
 */
public class SetChannelAudioOutputDevice extends EnhancedTask {
	private int channel;
	private int deviceID;
	
	/**
	 * Creates new instance of <code>SetChannelAudioOutputDevice</code>.
	 * @param channel The sampler channel number.
	 * @param deviceID The numerical ID of the audio output device.
	 */
	public
	SetChannelAudioOutputDevice(int channel, int deviceID) {
		setTitle("SetChannelAudioOutputDevice_task");
		setDescription (
			i18n.getMessage("SetChannelAudioOutputDevice.description", channel)
		);
		
		this.channel = channel;
		this.deviceID = deviceID;
	}
	
	/** The entry point of the task. */
	public void
	run() {
		try { CC.getClient().setChannelAudioOutputDevice(channel, deviceID); }
		catch(Exception x) {
			setErrorMessage(getDescription() + ": " + HF.getErrorMessage(x));
			CC.getLogger().log(Level.FINE, getErrorMessage(), x);
		}
	}
}
