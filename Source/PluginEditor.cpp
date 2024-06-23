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
		nullptr)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(800, 600);
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
#pragma region Text_Input
	addAndMakeVisible(textEditor);
	textEditor.addListener(this);
	textEditor.setTextToShowWhenEmpty("Enter text here...", juce::Colours::lightgrey);
	addAndMakeVisible(confirmButton);
	confirmButton.setButtonText("Confirm");
	confirmButton.addListener(this);
#pragma endregion
#pragma region Number_Of_Gens_Input
	addAndMakeVisible(numberOfGensDropdown);
	numberOfGensDropdown.addItem("1", 1);
	numberOfGensDropdown.addItem("2", 2);
	numberOfGensDropdown.addItem("3", 3);
	numberOfGensDropdown.addItem("4", 4);
	numberOfGensDropdown.addItem("5", 5);
	numberOfGensDropdown.addItem("6", 6);
	numberOfGensDropdown.addItem("7", 7);
	numberOfGensDropdown.addItem("8", 8);
	numberOfGensDropdown.onChange = [this] { comboBoxChanged(&numberOfGensDropdown); };
	numberOfGensDropdown.setSelectedId(1);
	addAndMakeVisible(numberOfGensLabel);
	numberOfGensLabel.setText("Number of Generations:", juce::dontSendNotification);
	numberOfGensLabel.attachToComponent(&numberOfGensDropdown, true);
#pragma endregion
#pragma region Scale_Input
	addAndMakeVisible(scaleDropdown);
	scaleDropdown.addItem("C major", 1);
	scaleDropdown.addItem("C minor", 2);
	scaleDropdown.addItem("C# major", 3);
	scaleDropdown.addItem("C# minor", 4);
	scaleDropdown.addItem("Db major", 5);
	scaleDropdown.addItem("Db minor", 6);
	scaleDropdown.addItem("D major", 7);
	scaleDropdown.addItem("D minor", 8);
	scaleDropdown.addItem("D# major", 9);
	scaleDropdown.addItem("D# minor", 10);
	scaleDropdown.addItem("Eb major", 11);
	scaleDropdown.addItem("Eb minor", 12);
	scaleDropdown.addItem("E major", 13);
	scaleDropdown.addItem("E minor", 14);
	scaleDropdown.addItem("F major", 15);
	scaleDropdown.addItem("F minor", 16);
	scaleDropdown.addItem("F# major", 17);
	scaleDropdown.addItem("F# minor", 18);
	scaleDropdown.addItem("Gb major", 19);
	scaleDropdown.addItem("Gb minor", 20);
	scaleDropdown.addItem("G major", 21);
	scaleDropdown.addItem("G minor", 22);
	scaleDropdown.addItem("G# major", 23);
	scaleDropdown.addItem("G# minor", 24);
	scaleDropdown.addItem("Ab major", 25);
	scaleDropdown.addItem("Ab minor", 26);
	scaleDropdown.addItem("A major", 27);
	scaleDropdown.addItem("A minor", 28);
	scaleDropdown.addItem("A# major", 29);
	scaleDropdown.addItem("A# minor", 30);
	scaleDropdown.addItem("Bb major", 31);
	scaleDropdown.addItem("Bb minor", 32);
	scaleDropdown.addItem("B major", 33);
	scaleDropdown.addItem("B minor", 34);
	scaleDropdown.onChange = [this] { comboBoxChanged(&scaleDropdown); };
	scaleDropdown.setSelectedId(1);
	addAndMakeVisible(scaleLabel);
	scaleLabel.setText("Scale:", juce::dontSendNotification);
	scaleLabel.attachToComponent(&scaleDropdown, true);
#pragma endregion
#pragma region File_Browser
	addAndMakeVisible(&fileBrowser);
	fileBrowser.addListener(this);
#pragma endregion
	//Keyboard
	addAndMakeVisible(&keyboardComponent);
#pragma region midi_note_shifting
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
	fileBrowser.removeListener(this);
}

//==============================================================================
void AiPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.setColour(juce::Colours::darkgreen);
	g.setFont(15.0f);
	//g.drawFittedText("SISILAFAMILLE", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
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
	numberOfGensDropdown.setBounds(4 * padding + dropdownWidth - 5, totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight, dropdownWidth, dropdownHeight);
	scaleDropdown.setBounds(4 * padding + dropdownWidth - 5 + 10 * padding, totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight, dropdownWidth, dropdownHeight);
	volumeKnob.setBounds(10, 10, 100, 100);
	midiInputList.setBounds(200, 10, totalWidth - 210, 20);
	keyboardComponent.setBounds(padding, totalHeight - keyboardHeight - padding, availableWidth, keyboardHeight);
	fileBrowser.setBounds(padding, totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight - fileBrowserHeight, availableWidth, fileBrowserHeight);
	textEditor.setBounds(padding, totalHeight - keyboardHeight - 2 * padding - textEditorHeight, textEditorWidth, textEditorHeight);
	confirmButton.setBounds(padding + textEditorWidth + padding, totalHeight - keyboardHeight - 2 * padding - textEditorHeight, wantedButtonWidth, textEditorHeight);

	shiftLeftButton.setBounds(scaleDropdown.getBounds().getX()+ scaleDropdown.getBounds().getWidth() + 50,
		scaleDropdown.getBounds().getY(),
		25,25);
	currentNoteLabel.setBounds(shiftLeftButton.getBounds().getX() + shiftLeftButton.getBounds().getWidth() + 25,
		scaleDropdown.getBounds().getY(),
		25, 25);
	shiftRightButton.setBounds(currentNoteLabel.getBounds().getX() + currentNoteLabel.getBounds().getWidth() + 25,
		scaleDropdown.getBounds().getY(),
		25, 25);
	currentNoteLabel.setText(audioProcessor.getSynthRootNote(), juce::dontSendNotification);
}


void AiPluginAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &midiInputList)
		setMidiInput(midiInputList.getSelectedItemIndex());
	else if (comboBoxThatHasChanged == &numberOfGensDropdown) {
		int selected = numberOfGensDropdown.getSelectedId();
		audioProcessor.setNumberOfGens(selected);
	}
	else if (comboBoxThatHasChanged == &scaleDropdown) {
		int selected = scaleDropdown.getSelectedId();
		juce::String scale = scaleDropdown.getItemText(selected-1);
		audioProcessor.setScale(scale);
	}
}
void AiPluginAudioProcessorEditor::selectionChanged()
{
	// Handle file selection change if needed
}
void AiPluginAudioProcessorEditor::fileClicked(const juce::File& file, const juce::MouseEvent& e)
{
	audioProcessor.playAndSetAudioFile(file);
	currentNoteLabel.setText(audioProcessor.getSynthRootNote(), juce::dontSendNotification);
	keyboardComponent.setWantsKeyboardFocus(true);
}

void AiPluginAudioProcessorEditor::fileDoubleClicked(const juce::File& file)
{
	// Handle file double-click if needed
}

void AiPluginAudioProcessorEditor::browserRootChanged(const juce::File& newRoot)
{
	// Handle root change if needed
}

void AiPluginAudioProcessorEditor::refreshFileBrowser()
{
	fileBrowser.refresh();
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
void AiPluginAudioProcessorEditor::textEditorTextChanged(juce::TextEditor& editor)
{
	if (&editor == &textEditor)
	{
		juce::String text = textEditor.getText();
		// Do something with the text, e.g., send it to the processor or handle it within the editor
	}
}
void AiPluginAudioProcessorEditor::buttonClicked(juce::Button* button)
{
	if (button == &confirmButton)
	{
		juce::String text = textEditor.getText();
		audioProcessor.setConfigAndPrompt(text);
		audioProcessor.launchGen();
	}
	else if (button == &shiftRightButton) {
		audioProcessor.decrementShift();
		currentNoteLabel.setText(juce::String(audioProcessor.getSynthRootNote()), juce::dontSendNotification);
	}
	else if (button == &shiftLeftButton) {
		audioProcessor.incrementShift();;
		currentNoteLabel.setText(juce::String(audioProcessor.getSynthRootNote()), juce::dontSendNotification);
	}
}