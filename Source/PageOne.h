
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class PageOne : public juce::Component,
    private juce::Button::Listener,
    private juce::ComboBox::Listener,
    private juce::TextEditor::Listener,
    public juce::FileBrowserListener {
public:
    PageOne(AiPluginAudioProcessor& processor);
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void selectionChanged() override;
    void fileClicked(const juce::File& file, const juce::MouseEvent& e) override;
    void fileDoubleClicked(const juce::File& file) override;
    void browserRootChanged(const juce::File& newRoot) override;
    void refreshFileBrowser();
    void removeListeners();
private:
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;

    juce::Slider volumeKnob;
    juce::ComboBox scaleDropdown;
    juce::Label scaleLabel;
    juce::TextEditor textEditor;
    juce::TextButton confirmButton;
    juce::ComboBox numberOfGensDropdown;
    juce::Label numberOfGensLabel;
    juce::FileBrowserComponent fileBrowser;
    AiPluginAudioProcessor& audioProcessor;
    juce::Label currentNoteLabel;

    void setupScaleDropdown();
    void setupNumberOfGensDropdown();
    void setupTextEditor();
    void setupFileBrowser();
};