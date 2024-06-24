#include "SynthAudioSource.h"

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState)
    : keyboardState(keyState), synthRootNote(523.25), detectedRootNoteFrequency(synthRootNote) {
    for (int i = 0; i < 4; ++i)
        synth.addVoice(new SineWaveVoice());
    synth.addSound(new SineWaveSound());
    formatManager.registerBasicFormats();
}

void SynthAudioSource::setUsingSineWaveSound() {
    synth.clearSounds();
}

void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    midiCollector.reset(sampleRate);
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void SynthAudioSource::releaseResources() {
    transportSource.releaseResources();
}

void SynthAudioSource::playAndSetAudioFile(const juce::File& file, bool play) {
    if (file.existsAsFile() && file.hasFileExtension("wav")) {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        auto* reader = formatManager.createReaderFor(file);
        if (reader) {
            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            detectedRootNoteFrequency = detectRootFrequency(reader);
            hasAudioSet = true;
            if (play)
                transportSource.start();
        }
        else {
            DBG("Error: Failed to create reader for file: " + file.getFullPathName());
        }
    }
}

juce::String SynthAudioSource::getSynthRootNote() {
    return getReadableNote(synthRootNote, semiTonShift);
}

juce::String SynthAudioSource::getDetectedNote() {
    return getReadableNote(detectedRootNoteFrequency);
}

double SynthAudioSource::detectRootFrequency(juce::AudioFormatReader* reader) {
    juce::AudioBuffer<float> buffer(reader->numChannels, reader->lengthInSamples);
    reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

    int order = 10;
    int size = 1 << order;
    juce::dsp::FFT fft(order);
    std::vector<float> window(size), fftData(size * 2);
    juce::dsp::WindowingFunction<float>::fillWindowingTables(window.data(), size, juce::dsp::WindowingFunction<float>::hann);

    for (int i = 0; i < size; ++i)
        fftData[i] = buffer.getSample(0, i) * window[i];

    fft.performFrequencyOnlyForwardTransform(fftData.data());
    auto peakIndex = std::distance(fftData.begin(), std::max_element(fftData.begin(), fftData.end()));
    double peakFrequency = peakIndex * reader->sampleRate / size;

    return peakFrequency;
}

void SynthAudioSource::incrementShift() {
    semiTonShift++;
}

void SynthAudioSource::decrementShift() {
    semiTonShift--;
}

void SynthAudioSource::applySemitonShift(juce::MidiBuffer& midiMessages) {
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
                processedMidi.addEvent(message, metadata.samplePosition);
            }
        }
    }
    midiMessages.swapWith(processedMidi);
}

void SynthAudioSource::stop() {
    transportSource.stop();
    transportSource.setNextReadPosition(0);
}

bool SynthAudioSource::isPlaying() {
    return transportSource.isPlaying();
}

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
    keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
    applySemitonShift(incomingMidi);
    if (!hasAudioSet) {
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }
    else {
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
                transportSource.start();
            }
        }
        transportSource.getNextAudioBlock(bufferToFill);
    }
}

juce::MidiMessageCollector* SynthAudioSource::getMidiCollector() {
    return &midiCollector;
}

double SynthAudioSource::midiNoteToFrequency(int midiNote) {
    constexpr double a = 440.0;
    return (a / 32) * std::pow(2, ((midiNote - 9) / 12.0));
}

juce::String SynthAudioSource::getReadableNote(double inputNote, int shiftOffset) {
    const std::array<std::string, 12> notes = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int semitonesFromC4 = std::round(12 * std::log2(inputNote / synthRootNote)) - shiftOffset;
    int noteIndex = (semitonesFromC4 % 12 + 12) % 12;
    int octave = (semitonesFromC4 / 12) + 4;
    return juce::String(notes[noteIndex]) + juce::String(octave);
}
