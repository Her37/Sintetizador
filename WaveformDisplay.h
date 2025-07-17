#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay();
    ~WaveformDisplay() override;

    void setBuffer(const juce::AudioBuffer<float>& bufferToDisplay);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioBuffer<float> audioBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
