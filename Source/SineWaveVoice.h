#include <JuceHeader.h>
#include "SineWaveSound.h"

class SineWaveVoice : public juce::SynthesiserVoice
{
public:
	bool canPlaySound(juce::SynthesiserSound* sound);
	void startNote(int midiNoteNumber, float velocity,
		juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;
	

	void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;

	void stopNote(float /*velocity*/, bool allowTailOff) override;

	void pitchWheelMoved(int) override {}
	void controllerMoved(int, int) override {}
private:
	double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};