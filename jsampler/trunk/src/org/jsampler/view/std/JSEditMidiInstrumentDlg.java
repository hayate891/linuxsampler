/*
 *   JSampler - a java front-end for LinuxSampler
 *
 *   Copyright (C) 2005-2007 Grigor Iliev <grigor@grigoriliev.com>
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

package org.jsampler.view.std;

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;

import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

import net.sf.juife.OkCancelDialog;

import org.jsampler.CC;

import org.linuxsampler.lscp.MidiInstrumentInfo;
import org.linuxsampler.lscp.SamplerEngine;

import static org.jsampler.view.std.StdI18n.i18n;


/**
 *
 * @author Grigor Iliev
 */
public class JSEditMidiInstrumentDlg extends OkCancelDialog {
	private final JLabel lName = new JLabel(i18n.getLabel("JSEditMidiInstrumentDlg.lName"));
	
	private final JLabel lFilename =
		new JLabel(i18n.getLabel("JSEditMidiInstrumentDlg.lFilename"));
	
	private final JLabel lIndex = new JLabel(i18n.getLabel("JSEditMidiInstrumentDlg.lIndex"));
	
	private final JLabel lEngine = new JLabel(i18n.getLabel("JSEditMidiInstrumentDlg.lEngine"));
	
	private final JLabel lLoadMode =
		new JLabel(i18n.getLabel("JSEditMidiInstrumentDlg.lLoadMode"));
	
	private final JLabel lVolume =
		new JLabel(i18n.getLabel("JSEditMidiInstrumentDlg.lVolume"));
	
	private final JTextField tfName = new JTextField();
	private final JTextField tfFilename = new JTextField();
	private final JSpinner spinnerIndex = new JSpinner(new SpinnerNumberModel(0, 0, 500, 1));
	private final JComboBox cbEngine = new JComboBox();
	private final JComboBox cbLoadMode = new JComboBox();
	private final JSlider slVolume = new JSlider(0, 100, 100);
	
	private final MidiInstrumentInfo instrument;
	
	/**
	 * Creates a new instance of <code>JSEditMidiInstrumentDlg</code>.
	 * 
	 * @param instr The instrument whose settings should be edited.
	 */
	public JSEditMidiInstrumentDlg(MidiInstrumentInfo instr) {
		super(CC.getMainFrame(), i18n.getLabel("JSEditMidiInstrumentDlg.title"));
		
		this.instrument = instr;
		
		GridBagLayout gridbag = new GridBagLayout();
		GridBagConstraints c = new GridBagConstraints();
		
		JPanel p = new JPanel();
		p.setLayout(gridbag);
		
		c.fill = GridBagConstraints.NONE;
		
		c.gridx = 0;
		c.gridy = 0;
		c.anchor = GridBagConstraints.EAST;
		c.insets = new Insets(3, 3, 3, 3);
		gridbag.setConstraints(lName, c);
		p.add(lName); 
		
		c.gridx = 0;
		c.gridy = 1;
		gridbag.setConstraints(lFilename, c);
		p.add(lFilename); 
		
		c.gridx = 0;
		c.gridy = 2;
		gridbag.setConstraints(lIndex, c);
		p.add(lIndex);
		
		c.gridx = 0;
		c.gridy = 3;
		c.insets = new Insets(12, 3, 3, 3);
		gridbag.setConstraints(lEngine, c);
		p.add(lEngine);
		
		c.gridx = 0;
		c.gridy = 4;
		c.insets = new Insets(3, 3, 3, 3);
		gridbag.setConstraints(lLoadMode, c);
		p.add(lLoadMode);
		
		c.gridx = 0;
		c.gridy = 5;
		gridbag.setConstraints(lVolume, c);
		p.add(lVolume);
		
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 1;
		c.gridy = 0;
		c.weightx = 1.0;
		c.insets = new Insets(3, 12, 3, 3);
		c.anchor = GridBagConstraints.WEST;
		gridbag.setConstraints(tfName, c);
		p.add(tfName);
		
		c.gridx = 1;
		c.gridy = 1;
		c.anchor = GridBagConstraints.WEST;
		gridbag.setConstraints(tfFilename, c);
		p.add(tfFilename);
			
		c.gridx = 1;
		c.gridy = 2;
		gridbag.setConstraints(spinnerIndex, c);
		p.add(spinnerIndex);
		
		c.gridx = 1;
		c.gridy = 3;
		c.insets = new Insets(12, 12, 3, 64);
		gridbag.setConstraints(cbEngine, c);
		p.add(cbEngine);
		
		c.gridx = 1;
		c.gridy = 4;
		c.insets = new Insets(3, 12, 3, 64);
		gridbag.setConstraints(cbLoadMode, c);
		p.add(cbLoadMode);
			
		c.gridx = 1;
		c.gridy = 5;
		gridbag.setConstraints(slVolume, c);
		p.add(slVolume);
		
		setMainPane(p);
		
		tfName.setText(instr.getName());
		tfFilename.setText(instr.getFilePath());
		spinnerIndex.setValue(instr.getInstrumentIndex());
		slVolume.setValue((int)(instr.getVolume() * 100));
		
		for(SamplerEngine e : CC.getSamplerModel().getEngines()) {
			cbEngine.addItem(e);
			if(e.getName().equals(instr.getEngine())) cbEngine.setSelectedItem(e);
		}
		
		cbLoadMode.addItem(MidiInstrumentInfo.LoadMode.DEFAULT);
		cbLoadMode.addItem(MidiInstrumentInfo.LoadMode.ON_DEMAND);
		cbLoadMode.addItem(MidiInstrumentInfo.LoadMode.ON_DEMAND_HOLD);
		cbLoadMode.addItem(MidiInstrumentInfo.LoadMode.PERSISTENT);
		cbLoadMode.setSelectedItem(instr.getLoadMode());
		
		tfName.getDocument().addDocumentListener(getHandler());
		tfFilename.getDocument().addDocumentListener(getHandler());
	}
	
	public MidiInstrumentInfo
	getInstrument() { return instrument; }
	
	public void
	onOk() {
		instrument.setName(tfName.getText());
		instrument.setFilePath(tfFilename.getText());
		instrument.setInstrumentIndex(Integer.parseInt(spinnerIndex.getValue().toString()));
		String s = ((SamplerEngine)cbEngine.getSelectedItem()).getName();
		instrument.setEngine(s);
		instrument.setLoadMode((MidiInstrumentInfo.LoadMode)cbLoadMode.getSelectedItem());
		float f = slVolume.getValue();
		f /= 100;
		instrument.setVolume(f);
		
		setCancelled(false);
		setVisible(false);
	}
	
	public void
	onCancel() { setVisible(false); }
	
	private void
	updateState() {
		boolean b = tfName.getText().length() != 0 && tfFilename.getText().length() != 0;
		btnOk.setEnabled(b);
	}
	
	private final Handler eventHandler = new Handler();
	
	private Handler
	getHandler() { return eventHandler; }
	
	private class Handler implements DocumentListener {
		// DocumentListener
		public void
		insertUpdate(DocumentEvent e) { updateState(); }
		
		public void
		removeUpdate(DocumentEvent e) { updateState(); }
		
		public void
		changedUpdate(DocumentEvent e) { updateState(); }
	}
}
