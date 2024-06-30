#pragma once
#include "PageOne.h"

PageOne::PageOne(AiPluginAudioProcessor& processor) : audioProcessor(processor),
	fileBrowser(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectDirectories,
		processor.loadPluginDocFolder(),
		new juce::WildcardFileFilter("*.wav", "*", "WAV files"),
		nullptr) {
	setupScaleDropdown();
	setupNumberOfGensDropdown();
	setupTextEditor();
	setupFileBrowser();
	addAndMakeVisible(&fileBrowser);
}

void PageOne::resized() {
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

	textEditor.setBounds(padding, totalHeight - keyboardHeight - 2 * padding - textEditorHeight, textEditorWidth, textEditorHeight);
	confirmButton.setBounds(padding + textEditorWidth + padding, totalHeight - keyboardHeight - 2 * padding - textEditorHeight, wantedButtonWidth, textEditorHeight);
	numberOfGensDropdown.setBounds(4 * padding + dropdownWidth - 5, totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight, dropdownWidth, dropdownHeight);
	scaleDropdown.setBounds(4 * padding + dropdownWidth - 5 + 10 * padding, totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight, dropdownWidth, dropdownHeight);
	fileBrowser.setBounds(padding, totalHeight - keyboardHeight - 3 * padding - textEditorHeight - dropdownHeight - fileBrowserHeight, availableWidth, fileBrowserHeight);

}


void PageOne::setupScaleDropdown() {
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
}

void PageOne::setupNumberOfGensDropdown() {
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
}

void PageOne::setupTextEditor() {
	// Setup code for textEditor
	addAndMakeVisible(textEditor);
	textEditor.addListener(this);
	textEditor.setTextToShowWhenEmpty("Enter text here...", juce::Colours::lightgrey);
	addAndMakeVisible(confirmButton);
	confirmButton.setButtonText("Confirm");
	confirmButton.addListener(this);
}

void PageOne::setupFileBrowser() {
	addAndMakeVisible(&fileBrowser);
	fileBrowser.addListener(this);
}

void PageOne::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &numberOfGensDropdown) {
		int selected = numberOfGensDropdown.getSelectedId();
		audioProcessor.setNumberOfGens(selected);
	}
	else if (comboBoxThatHasChanged == &scaleDropdown) {
		int selected = scaleDropdown.getSelectedId();
		juce::String scale = scaleDropdown.getItemText(selected - 1);
		audioProcessor.setScale(scale);
	}
}
void PageOne::selectionChanged()
{
	// Handle file selection change if needed
}
void PageOne::fileClicked(const juce::File& file, const juce::MouseEvent& e)
{
	audioProcessor.playAndSetAudioFile(file);
	currentNoteLabel.setText(audioProcessor.getSynthRootNote(), juce::dontSendNotification);
}

void PageOne::fileDoubleClicked(const juce::File& file)
{
	// Handle file double-click if needed
}

void PageOne::browserRootChanged(const juce::File& newRoot)
{
	// Handle root change if needed
}

void PageOne::refreshFileBrowser()
{
	fileBrowser.refresh();
}
void PageOne::removeListeners()
{
	fileBrowser.removeListener(this);

}
void PageOne::textEditorTextChanged(juce::TextEditor& editor)
{
	if (&editor == &textEditor)
	{
		juce::String text = textEditor.getText();
		// Do something with the text, e.g., send it to the processor or handle it within the editor
	}
}
void PageOne::buttonClicked(juce::Button* button)
{
	if (button == &confirmButton)
	{
		juce::String text = textEditor.getText();
		audioProcessor.setConfigAndPrompt(text);
		audioProcessor.launchGen();
	}
	
}
