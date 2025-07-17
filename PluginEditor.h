#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveformDisplay.h"
#include <array>



//==============================================================================
// Clase que representa la interfaz gráfica del plugin
class AcordeControlAudioProcessorEditor : public juce::AudioProcessorEditor,
private juce::Timer  // Para refrescar visualización
{                                         
public:
    // Constructor recibe una referencia al procesador para poder acceder a parámetros
    AcordeControlAudioProcessorEditor (AcordeControlAudioProcessor&);
    ~AcordeControlAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;  // Método para dibujar la interfaz
    void resized() override;                 // Método que se llama cuando se redimensiona la ventana

private:
    AcordeControlAudioProcessor& audioProcessor;  // Referencia al procesador para acceder a parámetros

    juce::Slider gainSlider;    // Slider para controlar la ganancia
    juce::Label gainLabel;      // Etiqueta para el slider
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;// Conexión entre el slider y el parámetro "gain" del procesador

    juce::Slider volSlider;    // Slider para controlar la ganancia
    juce::Label volLabel;      // Etiqueta para el slider
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAttachment;// Conexión entre el slider y el parámetro "gain" del procesador
    std::array<std::unique_ptr<juce::TextButton>, 5> trasteButtons;

    juce::Slider delayStepSlider;
    juce::Label delayStepLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayStepAttachment;


    WaveformDisplay waveformDisplay;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AcordeControlAudioProcessorEditor)
};
