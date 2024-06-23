/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h" 
#include "cmath"
//==============================================================================
AiPluginAudioProcessor::AiPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
	, synthAudioSource(keyboardState)
{
	volume = 127;
	deviceManager.initialise(0, 2, nullptr, true);
	formatManager.registerBasicFormats();
	loadScript();
}

AiPluginAudioProcessor::~AiPluginAudioProcessor()
{
	transportSource.setSource(nullptr);
}

//==============================================================================
const juce::String AiPluginAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool AiPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool AiPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool AiPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double AiPluginAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int AiPluginAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int AiPluginAudioProcessor::getCurrentProgram()
{
	return 0;
}

void AiPluginAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AiPluginAudioProcessor::getProgramName(int index)
{
	return {};
}

void AiPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void AiPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	synthAudioSource.prepareToPlay(samplesPerBlock, sampleRate);
	transportSource.prepareToPlay(samplesPerBlock, sampleRate);

}

void AiPluginAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	synthAudioSource.releaseResources();
	transportSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AiPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void AiPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	buffer.clear();
	juce::AudioPlayHead::CurrentPositionInfo positionInfo;
	if (auto* playHead = getPlayHead())
	{
		playHead->getCurrentPosition(positionInfo);
	}
	if (!positionInfo.isPlaying) {
		
	}
	if (hasAudioSet) {
		juce::MidiMessage message;
		int time;
		juce::MidiBuffer::Iterator it(midiMessages);
		while (it.getNextEvent(message, time)) {
			if (message.isNoteOn()) {
				double frequency = synthAudioSource.midiNoteToFrequency(message.getNoteNumber());
				double playbackSpeed = frequency / synthAudioSource.synthRootNote;
				transportSource.setNextReadPosition(0);
				transportSource.setSource(readerSource.get(), 0, nullptr, readerSource->getAudioFormatReader()->sampleRate* playbackSpeed);
				transportSource.start();
			}
			
		}
		juce::AudioBuffer<float> synthBuffer;
		synthBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
		synthBuffer.clear();
		juce::AudioSourceChannelInfo synthInfo(&synthBuffer, 0, buffer.getNumSamples());
		synthAudioSource.getNextAudioBlock(synthInfo);

		juce::AudioBuffer<float> transportBuffer;
		transportBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
		transportBuffer.clear();
		juce::AudioSourceChannelInfo transportInfo(&transportBuffer, 0, buffer.getNumSamples());
		transportSource.getNextAudioBlock(transportInfo);
		/*transportSource.getNextAudioBlock(bufferToFill);
		synthAudioSource.getNextAudioBlock(bufferToFill);*/
		for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
			buffer.addFrom(channel, 0, synthBuffer, channel, 0, buffer.getNumSamples());
			buffer.addFrom(channel, 0, transportBuffer, channel, 0, buffer.getNumSamples());
		}
	}
	else {
		juce::AudioSourceChannelInfo bufferToFill{ &buffer, 0, buffer.getNumSamples() };
		synthAudioSource.getNextAudioBlock(bufferToFill);
	}
	buffer.applyGain(volume);

}
void AiPluginAudioProcessor::playAndSetAudioFile(const juce::File& file)
{
	synthAudioSource.playAndSetAudioFile(file, true);
	if (file.existsAsFile() && file.hasFileExtension("wav"))
	{
		transportSource.stop();
		transportSource.setSource(nullptr);
		readerSource.reset();

		if (auto* reader = formatManager.createReaderFor(file))
		{
			readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
			transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
			hasAudioSet = true;
		}
		else
		{
			DBG("Error: Failed to create reader for file: " + file.getFullPathName());
		}
	}
}
void AiPluginAudioProcessor::incrementShift()
{
	synthAudioSource.incrementShift();
}
void AiPluginAudioProcessor::decrementShift()
{
	synthAudioSource.decrementShift();
}
juce::String AiPluginAudioProcessor::getDetectedNote()
{
	return synthAudioSource.getDetectedNote();
}

juce::String AiPluginAudioProcessor::getSynthRootNote() {
	return synthAudioSource.getSynthRootNote();
}
void AiPluginAudioProcessor::setConfigAndPrompt(const juce::String& prompt)
{
	juce::File configFile = loadConfig();
	juce::String finalOuput = "";
	if (auto* playHead = getPlayHead())
	{
		juce::AudioPlayHead::CurrentPositionInfo positionInfo;
		if (playHead->getCurrentPosition(positionInfo))
		{
			double roundedBpm = std::round(positionInfo.bpm);
			finalOuput =
				"{\r\n"
				"    \"BPM\": " + juce::String(roundedBpm) + ", \r\n"
				"    \"timeSigNumerator\": " + juce::String(positionInfo.timeSigNumerator) + ", \r\n"
				"    \"timeSigDenominator\":" + juce::String(positionInfo.timeSigNumerator) + ", \r\n"
				"    \"numberOfGens\": " + juce::String(numberOfGens) + ", \r\n"
				"    \"scale\": \"" + selectedScale + "\"" + ", \r\n"
				"    \"prompt\": \"" + prompt + "\"" + " \r\n"
				"}";
		}
	}

	configFile.replaceWithText(finalOuput);
}
void AiPluginAudioProcessor::launchGen()
{
	juce::File pythonScript = loadScript();
	juce::File documentsDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	juce::File aiPluginFilesDirectory = documentsDirectory.getChildFile("AiPluginFiles");

	juce::String cdToPluginFiles = "cd " + aiPluginFilesDirectory.getFullPathName() + " ";
	juce::String cmdInit = "conda init cmd.exe ";
	juce::String activateEnv = "conda activate MusicGen ";
	juce::String runScript = "python " + pythonScript.getFullPathName() + " ";

	juce::String command = "cmd.exe /C \"" + cdToPluginFiles + +"&& " + cmdInit + "&& " + activateEnv + "&& " + runScript + "\"";
	std::system(command.toRawUTF8());
	if (auto* editor = dynamic_cast<AiPluginAudioProcessorEditor*>(getActiveEditor()))
	{
		juce::MessageManager::callAsync([editor] {
			editor->fileBrowser.refresh();
			});
	}
}

void AiPluginAudioProcessor::setNumberOfGens(int number)
{
	numberOfGens = number;
}

void AiPluginAudioProcessor::setScale(juce::String scale)
{
	selectedScale = scale;
}

juce::File AiPluginAudioProcessor::loadPluginDocFolder()
{
	juce::File documentsDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	juce::File aiPluginFilesDirectory = documentsDirectory.getChildFile("AiPluginFiles");
	if (!aiPluginFilesDirectory.exists())
	{
		aiPluginFilesDirectory.createDirectory();
	}
	return aiPluginFilesDirectory;
}
juce::File AiPluginAudioProcessor::loadScript()
{
	juce::File aiPluginFilesDirectory = loadPluginDocFolder();

	juce::File pythonScript = aiPluginFilesDirectory.getChildFile("Generate.py");

	if (!pythonScript.exists())
	{
		pythonScript.replaceWithData(BinaryData::Generate_py, BinaryData::Generate_pySize);
	}
	return pythonScript;
}
juce::File AiPluginAudioProcessor::loadConfig()
{
	juce::File aiPluginFilesDirectory = loadPluginDocFolder();
	juce::File configFile = aiPluginFilesDirectory.getChildFile("config.json");
	if (!configFile.exists())
	{
		configFile.replaceWithData(BinaryData::config_json, BinaryData::config_jsonSize);

	}
	return configFile;
}
//==============================================================================
bool AiPluginAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AiPluginAudioProcessor::createEditor()
{
	return new AiPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AiPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void AiPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new AiPluginAudioProcessor();
}
