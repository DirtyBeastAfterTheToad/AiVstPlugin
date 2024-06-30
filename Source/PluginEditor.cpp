/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AiPluginAudioProcessorEditor::AiPluginAudioProcessorEditor(AiPluginAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p),
	keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
	fileBrowser(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectDirectories,
		audioProcessor.loadPluginDocFolder(),
		new juce::WildcardFileFilter("*.wav", "*", "WAV files"),
		nullptr),
	pageOne(audioProcessor)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(800, 600);
	addAndMakeVisible(&pageOne);
#pragma region Volume_Knob
	//volume  knob:
	volumeKnob.setSliderStyle(juce::Slider::Rotary);
	volumeKnob.setLookAndFeel(&customLookAndFeel);
	volumeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
	volumeKnob.setValue(1.0);
	volumeKnob.setRotaryParameters(0, 2 * 3.14159f, true);
	volumeKnob.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue);
	volumeKnob.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkblue);
	addAndMakeVisible(&volumeKnob);
	volumeKnob.addListener(this);
#pragma endregion
#pragma region Midi_Input
	//Midi Input
	addAndMakeVisible(midiInputListLabel);
	midiInputListLabel.setText("MIDI Input:", juce::dontSendNotification);
	midiInputListLabel.attachToComponent(&midiInputList, true);

	auto midiInputs = juce::MidiInput::getAvailableDevices();
	addAndMakeVisible(midiInputList);
	midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");

	juce::StringArray midiInputNames;
	for (auto input : midiInputs)
		midiInputNames.add(input.name);

	midiInputList.addItemList(midiInputNames, 1);
	midiInputList.onChange = [this] { setMidiInput(midiInputList.getSelectedItemIndex()); };

	for (auto input : midiInputs)
	{
		if (audioProcessor.deviceManager.isMidiInputDeviceEnabled(input.identifier))
		{
			setMidiInput(midiInputs.indexOf(input));
			break;
		}
	}
	if (midiInputList.getSelectedId() == 0)
		setMidiInput(0);
#pragma endregion 

	//Keyboard
	addAndMakeVisible(&keyboardComponent);
#pragma region midi_note_shifting
	currentNoteLabel.setText(audioProcessor.getSynthRootNote(), juce::dontSendNotification);
	addAndMakeVisible(currentNoteLabel);
	addAndMakeVisible(shiftLeftButton);
	shiftLeftButton.setButtonText("<");
	shiftLeftButton.addListener(this);

	addAndMakeVisible(shiftRightButton);
	shiftRightButton.setButtonText(">");
	shiftRightButton.addListener(this);
	addAndMakeVisible(currentNoteLabel);

#pragma endregion

}

AiPluginAudioProcessorEditor::~AiPluginAudioProcessorEditor()
{
	pageOne.removeListeners();
}

//==============================================================================
void AiPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.setColour(juce::Colours::darkgreen);
	g.setFont(15.0f);
}

void AiPluginAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	const int padding = 20;
	const int totalWidth = getWidth();
	const int totalHeight = getHeight();
	const int keyboardHeight = totalHeight / 4;
	const int availableWidth = totalWidth - 2 * padding;
	const int wantedButtonWidth = 0.1 * availableWidth;
	const int dropdownHeight = 20;
	const int dropdownWidth = 100;
	const int fileBrowserHeight = 200;
	const int textEditorHeight = 60;
	int textEditorWidth = availableWidth - wantedButtonWidth - padding;
	volumeKnob.setBounds(10, 10, 100, 100);
	midiInputList.setBounds(200, 10, totalWidth - 210, 20);
	keyboardComponent.setBounds(padding, totalHeight - keyboardHeight - padding, availableWidth, keyboardHeight);
	int noteShiftX = 4 * padding + dropdownWidth - 5 + 10 * padding;
	int noteShiftY = totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight;
	shiftLeftButton.setBounds(noteShiftX+ dropdownWidth+ 50,
		noteShiftY,
		25,25);
	currentNoteLabel.setBounds(shiftLeftButton.getBounds().getX() + shiftLeftButton.getBounds().getWidth() + 25,
		noteShiftY,
		25, 25);
	shiftRightButton.setBounds(currentNoteLabel.getBounds().getX() + currentNoteLabel.getBounds().getWidth() + 25,
		noteShiftY,
		25, 25);
	pageOne.setBounds(getLocalBounds());
}


void AiPluginAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &midiInputList)
		setMidiInput(midiInputList.getSelectedItemIndex());
	
}

void AiPluginAudioProcessorEditor::setMidiInput(int index)
{
	auto list = juce::MidiInput::getAvailableDevices();

	deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier,
		audioProcessor.synthAudioSource.getMidiCollector());

	auto newInput = list[index];

	if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
		deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

	deviceManager.addMidiInputDeviceCallback(newInput.identifier, audioProcessor.synthAudioSource.getMidiCollector());
	midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

	lastInputIndex = index;
}


void AiPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &volumeKnob)
	{
		audioProcessor.setGain(volumeKnob.getValue());
	}
}

void AiPluginAudioProcessorEditor::buttonClicked(juce::Button* button)
{
	
	if (button == &shiftRightButton) {
		audioProcessor.decrementShift();
		currentNoteLabel.setText(juce::String(audioProcessor.getSynthRootNote()), juce::dontSendNotification);
	}
	else if (button == &shiftLeftButton) {
		audioProcessor.incrementShift();;
		currentNoteLabel.setText(juce::String(audioProcessor.getSynthRootNote()), juce::dontSendNotification);
	}
}