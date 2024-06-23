#include <JuceHeader.h>
class CustomMidiKeyboardComponent : public juce::MidiKeyboardComponent {
public:
    CustomMidiKeyboardComponent(juce::MidiKeyboardState& state, Orientation orientation)
        : juce::MidiKeyboardComponent(state, orientation) {
        setKeyWidth(40.0); // Increase key width for larger keys
    }

protected:
    // Override to customize the appearance of white keys
    void drawWhiteNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
        bool isDown, bool isOver, juce::Colour lineColour, juce::Colour textColour) override {
        auto baseColour = isDown ? juce::Colours::darkgrey : juce::Colours::white;
        g.setColour(baseColour.withAlpha(isOver ? 0.9f : 1.0f));
        g.fillRect(area);

        g.setColour(lineColour);
        g.drawRect(area);

        if (isOver) {
            g.setColour(juce::Colours::darkgrey);
            g.drawRect(area, 1.0f); 
        }
        // Optional: Add text or additional graphics on the key
        g.setColour(textColour);
        g.setFont(12.0f);
        g.drawText(juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, true, 3),
            area, juce::Justification::centredBottom, false);
    }

    // Override to customize the appearance of black keys
    void drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
        bool isDown, bool isOver, juce::Colour noteFillColour) override {
        auto baseColour = isDown ? juce::Colours::darkgrey : juce::Colours::black;
        g.setColour(baseColour.withAlpha(isOver ? 0.9f : 1.0f));
        g.fillRect(area);

        if (isOver) {
            g.setColour(juce::Colours::darkgrey);
            g.drawRect(area, 1.0f); // highlight key edges when hovered
        }
    }
};
