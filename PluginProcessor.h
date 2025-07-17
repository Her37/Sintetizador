#pragma once

#include <JuceHeader.h>


//==============================================================================
class AcordeControlAudioProcessor  : public juce::AudioProcessor
{
public:
    AcordeControlAudioProcessor();
    ~AcordeControlAudioProcessor() override;

    // Procesamiento
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Editor
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // Información
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Parámetro y gestión de parámetros
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* gainParam = nullptr;
    std::atomic<float>* volParam = nullptr;

    std::atomic<float>* delayStepParam = nullptr;

	//void setGain(float g) { gainParam->store(g); }

    void setTraste(int t);  // declaración
	std::atomic<float>* trasteParam = nullptr;
    std::atomic<int> trasteActual = 0;

    //void setTraste(int t) { trasteActual = t; }

    juce::AudioBuffer<float> visualizerBuffer; // Buffer que contiene audio para el display
    
private:


    // Variables para generar sonido de acorde con rasgueo:
    const double pi = 3.14159265358979323846;
    const int sampleRateConst = 44100;  // Este valor se actualizará en prepareToPlay

    // Frecuencias de las notas para el rasgueo
    std::vector<double> freqs = {
        110.00, 164.81, 220.00, 277.18, 329.63,
        329.63, 277.18, 220.00, 164.81, 110.00,
        110.00, 164.81, 220.00, 277.18, 329.63
    };
    const int numTonos = sizeof(freqs) / sizeof(double);
    
    // Parámetros del rasgueo
    const int notasPorRasgueo = 5;
    const double delayStep = 0.1;
    const double delayOffset = delayStep * notasPorRasgueo + 0.1;
    const double tiempoDeCola = 1.0;
    double duration;  // duración total en segundos

    // Delays para cada nota en el rasgueo
    std::vector<double> delays;

    // Armónicos para el timbre
    std::vector<double> harmonicsAmps = { 1.0, 0.5, 0.25, 0.125 };

    // Contador para muestras procesadas (para saber el tiempo)
    int currentSample = 0;

    // Ganancia global y amplitud máxima
    const double globalGain = 0.3;
    const int amplitude = 32730;

    //const double distortionAmount = 1; // 1.0 = sin distorsión, >1.0 más distorsión

    double softClip(double x) {
        if (x > 1.0) return 2.0 / 3.0;
        else if (x < -1.0) return -2.0 / 3.0;
        else return x - (x * x * x) / 3.0;
    }

    // Funciones de envolvente (guitarra y piano)
    double envelopeGuitarra(double t);
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AcordeControlAudioProcessor)
};
