#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h> 
#include "SineWaveVoice.h"
class SynthAudioSource : public juce::AudioSource
{
public:
	SynthAudioSource(juce::MidiKeyboardState& keyState)
		: keyboardState(keyState)

	{
		for (auto i = 0; i < 4; ++i)
			synth.addVoice(new SineWaveVoice());

		synth.addSound(new SineWaveSound());
		formatManager.registerBasicFormats();
		detectedRootNoteFrequency = synthRootNote;
	}
	double synthRootNote = 523.25;
	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate);
		midiCollector.reset(sampleRate);
		transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	}

	void releaseResources() override
	{
		transportSource.releaseResources();
	}

	void playAndSetAudioFile(const juce::File& file, bool play) {
		if (file.existsAsFile() && file.hasFileExtension("wav"))
		{
			transportSource.stop();
			transportSource.setSource(nullptr);
			readerSource.reset();

			if (auto* reader = formatManager.createReaderFor(file))
			{
				readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
				transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
				detectedRootNoteFrequency=detectRootFrequency(reader);
				hasAudioSet = true;
				if (play)
					transportSource.start();
			}
			else
			{
				DBG("Error: Failed to create reader for file: " + file.getFullPathName());
			}
		}
	}
	juce::String getSynthRootNote() {

		return getReadableNote(synthRootNote, semiTonShift);
	}
	juce::String getDetectedNote() {
		
		return getReadableNote(detectedRootNoteFrequency);
	}

	double detectRootFrequency(juce::AudioFormatReader* reader) {
		juce::AudioBuffer<float> buffer(reader->numChannels, reader->lengthInSamples);
		reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

		//FFT
		int order = 10;
			// bitwise shifting
			//			1 << 0 = `0000 0001`
			//			1 << 1 = `0000 0010`
			//			1 << 2 = `0000 0100` == 1024 xDD
		int size = 1 << order;
		juce::dsp::FFT fft(order);
		//vector ~= dynamic arrays to store tape, then fft . fftdata contains the volume of the given frequencies
		//In a way, it's a bandpass filter which is applied to the whole frequencies, you take the ones where the volume is highest
		std::vector<float> window(size), fftData(size * 2);
		// Create a window function to taper the signal
		juce::dsp::WindowingFunction<float>::fillWindowingTables(window.data(), size, juce::dsp::WindowingFunction<float>::hann);
		// Applies the window function to the first size samples of the buffer and prepares the data for the FFT.
		for (int i = 0; i < size; ++i)
			fftData[i] = buffer.getSample(0, i) * window[i];
		//Performs the FFT
		fft.performFrequencyOnlyForwardTransform(fftData.data());
		//finds the index of the maximum value in the fftData, which corresponds to the peak frequency in the spectrum.
		auto peakIndex = std::distance(fftData.begin(), std::max_element(fftData.begin(), fftData.end()));
		//Converts the peak index to an actual frequency in Hz.
		double peakFrequency = peakIndex * reader->sampleRate / size;

		return peakFrequency;

	}
	void incrementShift() {
		semiTonShift ++;  
	}

	void decrementShift() {
		semiTonShift --;  
	}
	void applySemitonShift(juce::MidiBuffer& midiMessages) {
		juce::MidiBuffer processedMidi;
		for (const auto metadata : midiMessages) {
			auto message = metadata.getMessage();
			int newNoteNumber = message.getNoteNumber() + semiTonShift;
			if (newNoteNumber >= 0 && newNoteNumber <= 127) {
				if (message.isNoteOn()) {
					processedMidi.addEvent(juce::MidiMessage::noteOn(message.getChannel(), newNoteNumber, message.getVelocity()), metadata.samplePosition);
				}
				else if (message.isNoteOff()) {
					processedMidi.addEvent(juce::MidiMessage::noteOff(message.getChannel(), newNoteNumber, message.getVelocity()), metadata.samplePosition);
				}
				else {
					processedMidi.addEvent(message, metadata.samplePosition);  // Pass through other MIDI messages unchanged
				}
			}
		}
		midiMessages.swapWith(processedMidi);  // Replace the original MIDI buffer with the processed one
	}
	void stop() {
		transportSource.stop();
		transportSource.setNextReadPosition(0);
	}
	bool isPlaying() {
		return transportSource.isPlaying();
	}
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
	{
		// Fill the audio buffer with the synthesized sound
		bufferToFill.clearActiveBufferRegion();
		// Add your synthesis code here

		juce::MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
		keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
			bufferToFill.numSamples, true);
		applySemitonShift(incomingMidi);
		if (!hasAudioSet) {
			synth.renderNextBlock(*bufferToFill.buffer, incomingMidi,
				bufferToFill.startSample, bufferToFill.numSamples);
			return;
		}
		double frequency = 0.0;
		int noteNumber = 0;
		bool noteDetected = false; 

		for (const auto metadata : incomingMidi) {
			auto message = metadata.getMessage();
			if (message.isNoteOn()) {
				noteDetected = true;
				noteNumber = message.getNoteNumber();
				frequency = midiNoteToFrequency(noteNumber);
				double playbackSpeed = frequency / synthRootNote;
				transportSource.setNextReadPosition(0);
				transportSource.setSource(readerSource.get(), 0, nullptr, readerSource->getAudioFormatReader()->sampleRate * playbackSpeed);
				transportSource.start();  // Start the pre-loaded file playback
			}
			
		}
		transportSource.getNextAudioBlock(bufferToFill);
	}
	
	juce::MidiMessageCollector* getMidiCollector()
	{
		return &midiCollector;
	}
	double midiNoteToFrequency(int midiNote) {
		constexpr double a = 440.0; 
		return (a / 32) * std::pow(2, ((midiNote - 9) / 12.0));
	}

private:
	juce::MidiKeyboardState& keyboardState;
	juce::Synthesiser synth;
	juce::MidiMessageCollector midiCollector;
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
	juce::File currentPlaybackFile;
	juce::Array<int> currentPlayedNotes;

	double detectedRootNoteFrequency;
	int semiTonShift = 0;
	juce::String getReadableNote(double inputNote, int shiftOffset=0) {
		const std::array<std::string, 12> notes = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

		int semitonesFromC4 = std::round(12 * std::log2(inputNote / synthRootNote))-shiftOffset;
		int noteIndex = (semitonesFromC4 % 12 + 12) % 12;  // Normalize index to be within 0-11
		int octave = (semitonesFromC4 / 12) + 4;  // Calculate octave

		return juce::String(notes[noteIndex]) + juce::String(octave);
	}
	bool hasAudioSet = false;
};

