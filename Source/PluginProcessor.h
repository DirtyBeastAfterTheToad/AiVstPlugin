/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthAudioSource.h" 
//==============================================================================
/**
*/
class AiPluginAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	AiPluginAudioProcessor();
	~AiPluginAudioProcessor() override;
	float getVolume() { return volume; }
	void setGain(float newVolume) { volume = newVolume; }
	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;
	juce::MidiKeyboardState keyboardState;
	SynthAudioSource synthAudioSource{ keyboardState };
	juce::AudioDeviceManager deviceManager;
	void setConfigAndPrompt(const juce::String& text);
	void launchGen();
	void setNumberOfGens(int number);
	void setScale(juce::String scale);
	juce::File loadPluginDocFolder();
	void playAndSetAudioFile(const juce::File& file);
	juce::String getDetectedNote();
	juce::String getSynthRootNote();
	void incrementShift();
	void decrementShift();
	
private:
	//==============================================================================
	float volume;
	int numberOfGens;
	bool hasAudioSet = false;
	bool wasPlaying = false;
	juce::String selectedScale;
	juce::File loadScript();
	juce::File loadConfig();
	juce::AudioFormatManager formatManager;
	juce::AudioTransportSource transportSource;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AiPluginAudioProcessor);


};
