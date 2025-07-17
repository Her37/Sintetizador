#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay()
{
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::setBuffer(const juce::AudioBuffer<float>& bufferToDisplay)
{
    audioBuffer.makeCopyOf(bufferToDisplay);
    repaint();
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);

    auto width  = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();

    if (audioBuffer.getNumSamples() == 0)
        return;

    auto* channelData = audioBuffer.getReadPointer(0);
    auto numSamples = audioBuffer.getNumSamples();

    juce::Path waveformPath;
    waveformPath.startNewSubPath(0, height / 2);

    for (int i = 0; i < width; ++i)
    {
        auto sampleIndex = juce::jmap(i, 0, width - 1, 0, numSamples - 1);
        float sampleValue = channelData[sampleIndex];

        float y = juce::jmap(sampleValue, -1.0f, 1.0f, (float)height, 0.0f);
        waveformPath.lineTo((float)i, y);
    }

    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
}

void WaveformDisplay::resized()
{
}
