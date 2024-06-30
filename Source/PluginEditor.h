/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "CustomMidiKeyboardComponent.h"
#include "PageOne.h"
#include "PageTwo.h"
//==============================================================================
/**
*/
class AiPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
	private juce::ComboBox::Listener,
	private juce::Slider::Listener,
	private juce::Button::Listener
{
public:
	AiPluginAudioProcessorEditor(AiPluginAudioProcessor&);
	~AiPluginAudioProcessorEditor() override;

	//==============================================================================
	void buttonClicked(juce::Button* button) override;
	void paint(juce::Graphics&) override;
	void resized() override;
	juce::FileBrowserComponent fileBrowser;
private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	juce::MidiKeyboardState keyboardState;
	CustomMidiKeyboardComponent keyboardComponent;
	juce::Slider volumeKnob;
	//midi selection box
	juce::ComboBox midiInputList;
	juce::Label midiInputListLabel;
	int lastInputIndex = 0;
	juce::AudioDeviceManager deviceManager;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	void setMidiInput(int index);
	juce::TextButton confirmButton;
	CustomLookAndFeel customLookAndFeel;
	AiPluginAudioProcessor& audioProcessor;
	void sliderValueChanged(juce::Slider* slider) override;
	juce::Array<int> activeNotes;
	//Midi note shifting
	juce::TextButton shiftLeftButton;
	juce::TextButton shiftRightButton;
	juce::Label currentNoteLabel;
	PageOne pageOne;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AiPluginAudioProcessorEditor)
};
