#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WaveformDisplay.h"



//==============================================================================

AcordeControlAudioProcessorEditor::AcordeControlAudioProcessorEditor (AcordeControlAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Configuramos el rango del slider (de 0.0 a 1.0, paso 0.01)
    gainSlider.setRange(0.0, 1.0, 0.01);

    // Inicializamos el slider con el valor inicial del parámetro gain
    gainSlider.setValue(0.3);

    // Configuramos estilo del slider y el textbox que muestra el valor
    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);

    // Hacemos visible el slider en la ventana
    addAndMakeVisible(gainSlider);

    // Configuramos y mostramos la etiqueta del slider
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);

    // Conectamos el slider con el parámetro "gain" del procesador
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "gain", gainSlider);

    // Configuramos el rango del slider (de 0.0 a 1.0, paso 0.01)
    volSlider.setRange(0.0, 1.0, 0.1);

    // Inicializamos el slider con el valor inicial del parámetro gain
    volSlider.setValue(0.5);

    // Configuramos estilo del slider y el textbox que muestra el valor
    volSlider.setSliderStyle(juce::Slider::LinearVertical);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);

    // Hacemos visible el slider en la ventana
    addAndMakeVisible(volSlider);

    // Configuramos y mostramos la etiqueta del slider
    volLabel.setText("Vol", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, false);
    addAndMakeVisible(volLabel);

    // Conectamos el slider con el parámetro "gain" del procesador
    volAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "vol", volSlider);

    delayStepSlider.setRange(0.01, 0.5, 0.01);
    delayStepSlider.setValue(0.1);
    delayStepSlider.setSliderStyle(juce::Slider::LinearVertical);
    delayStepSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);

    addAndMakeVisible(delayStepSlider);

    delayStepLabel.setText("Delay", juce::dontSendNotification);
    delayStepLabel.attachToComponent(&delayStepSlider, false);
    addAndMakeVisible(delayStepLabel);

    delayStepAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "delaystep", delayStepSlider);


    trasteButtons = {
    std::make_unique<juce::TextButton>("LA"),
    std::make_unique<juce::TextButton>("SI"),
    std::make_unique<juce::TextButton>("DO"),
    std::make_unique<juce::TextButton>("RE"),
    std::make_unique<juce::TextButton>("MI")
    };

    for (int i = 0; i < trasteButtons.size(); ++i)
    {
        trasteButtons[i]->setRadioGroupId(1);
        addAndMakeVisible(*trasteButtons[i]);

        int traste = i;
        trasteButtons[i]->onClick = [this, traste]() {
            audioProcessor.setTraste(traste);
            };
    }



    addAndMakeVisible(waveformDisplay);
    startTimerHz(30); // refrescar 30 veces por segundo

    // Definimos tamaño de la ventana del plugin
    setSize(450, 400);
}

AcordeControlAudioProcessorEditor::~AcordeControlAudioProcessorEditor()
{
    stopTimer();
}

void AcordeControlAudioProcessorEditor::timerCallback()
{
    waveformDisplay.setBuffer(audioProcessor.visualizerBuffer);
}

void AcordeControlAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Sintetizador de Guitarra", getLocalBounds().removeFromTop(20), juce::Justification::centred, 1);
}

void AcordeControlAudioProcessorEditor::resized()
{
    const int sliderWidth = 60;
    const int sliderHeight = 150;
    const int sliderTop = 80;
    const int sliderSpacing = 20;
    const int leftMargin = 20;

    gainSlider.setBounds(leftMargin, sliderTop, sliderWidth, sliderHeight);
    volSlider.setBounds(leftMargin + (sliderWidth + sliderSpacing), sliderTop, sliderWidth, sliderHeight);
    delayStepSlider.setBounds(leftMargin + 2 * (sliderWidth + sliderSpacing), sliderTop, sliderWidth, sliderHeight);

    const int buttonWidth = 75;
    const int buttonHeight = 30;
    const int totalButtonWidth = static_cast<int>(trasteButtons.size()) * (buttonWidth + 10) - 10;
    const int startX = (getWidth() - totalButtonWidth) / 2;
    const int y = 20;

    for (int i = 0; i < trasteButtons.size(); ++i)
    {
        trasteButtons[i]->setBounds(startX + i * (buttonWidth + 10), y, buttonWidth, buttonHeight);
    }

    waveformDisplay.setBounds(getWidth() - 200, 80, 180, 150); 
}

//==============================================================================

