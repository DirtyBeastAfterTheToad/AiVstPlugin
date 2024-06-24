#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "SineWaveVoice.h"

class SynthAudioSource : public juce::AudioSource {
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState);
    void setUsingSineWaveSound();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void playAndSetAudioFile(const juce::File& file, bool play);
    juce::String getSynthRootNote();
    juce::String getDetectedNote();
    double detectRootFrequency(juce::AudioFormatReader* reader);
    void incrementShift();
    void decrementShift();
    void applySemitonShift(juce::MidiBuffer& midiMessages);
    void stop();
    bool isPlaying();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    juce::MidiMessageCollector* getMidiCollector();
    double midiNoteToFrequency(int midiNote);
    double synthRootNote = 523.25;

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
    juce::MidiMessageCollector midiCollector;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    double detectedRootNoteFrequency;
    int semiTonShift = 0;
    bool hasAudioSet = false;
    juce::String getReadableNote(double inputNote, int shiftOffset = 0);
};
