#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
	CustomLookAndFeel()
	{
		setColour(juce::Slider::thumbColourId, juce::Colours::darkgrey);
	}

	void drawRotarySlider(juce::Graphics& g,
		int x, int y, int width, int height,
		float sliderPosProportional,
		float rotaryStartAngle,
		float rotaryEndAngle,
		juce::Slider& slider) override
	{
		auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
		auto centreX = (float)x + (float)width * 0.5f;
		auto centreY = (float)y + (float)height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

		// Fill
		g.setColour(juce::Colours::darkgrey);
		g.fillEllipse(rx, ry, rw, rw);

		// Outline
		g.setColour(juce::Colours::black);
		g.drawEllipse(rx, ry, rw, rw, 1.0f);

		juce::Path p;
		auto pointerLength = radius * 0.6f;
		auto pointerThickness = 2.0f;
		p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

		// Pointer
		g.setColour(juce::Colours::white);
		g.fillPath(p);
	}
};