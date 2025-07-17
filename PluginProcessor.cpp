#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
// Constructor: inicializamos 'parameters' con el parámetro 'gain'
AcordeControlAudioProcessor::AcordeControlAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters(*this, nullptr, "PARAMETERS", {
           std::make_unique<juce::AudioParameterFloat>("gain",  // ID del parámetro
                                                       "Gain",  // Nombre del parámetro (para GUI)
                                                       1.0f,    // Valor mínimo
                                                       10.0f,    // Valor máximo
                                                       1.0f),    // Valor inicial por defecto
           std::make_unique<juce::AudioParameterFloat>("vol",  // ID del parámetro
                                                       "Vol",  // Nombre del parámetro (para GUI)
                                                       0.0f,    // Valor mínimo
                                                       10.0f,    // Valor máximo
                                                       1.0f),    // Valor inicial por defecto
            std::make_unique<juce::AudioParameterFloat>("delaystep", "DelayStep", 0.02f, 0.10f, 0.1f),


       })
#endif
{
    gainParam = parameters.getRawParameterValue("gain");  // Guardamos puntero para acceso rápido
    volParam = parameters.getRawParameterValue("vol");
    delayStepParam = parameters.getRawParameterValue("delaystep");


}

AcordeControlAudioProcessor::~AcordeControlAudioProcessor()
{
}

void AcordeControlAudioProcessor::setTraste(int t)
{
    trasteActual = t;
}

//==============================================================================

const juce::String AcordeControlAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AcordeControlAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AcordeControlAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AcordeControlAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AcordeControlAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AcordeControlAudioProcessor::getNumPrograms()
{
    return 1;
}

int AcordeControlAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AcordeControlAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AcordeControlAudioProcessor::getProgramName (int index)
{
    return {};
}

void AcordeControlAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}


void AcordeControlAudioProcessor::releaseResources()
{
    // Liberar recursos si es necesario al parar reproducción
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AcordeControlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Soportamos solo mono o estéreo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



bool AcordeControlAudioProcessor::hasEditor() const
{
    return true; // Indicamos que sí hay interfaz gráfica
}

juce::AudioProcessorEditor* AcordeControlAudioProcessor::createEditor()
{
    return new AcordeControlAudioProcessorEditor (*this);
}

void AcordeControlAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Guardar estado de parámetros en XML para preservar configuración
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AcordeControlAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Cargar estado de parámetros desde XML
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// Esta función es requerida por JUCE para crear una instancia del plugin.
// Si no está definida, da error de símbolo externo no resuelto.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AcordeControlAudioProcessor();
}

// Envolvente para la guitarra (exponencial)
double AcordeControlAudioProcessor::envelopeGuitarra(double t)
{
    return exp(-2.0 * t);
}

void AcordeControlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    currentSample = 0;
}

void AcordeControlAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Limpiar canales extras
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Ganancia del parámetro (valor actualizado)
    float distortionAmount = gainParam->load();
    float Volumen = volParam->load();
    int numSamples = buffer.getNumSamples();

    double delayStepValue = delayStepParam->load();
    delays.resize(freqs.size());
    for (int i = 0; i < freqs.size(); ++i)
        delays[i] = i * delayStepValue;

    duration = delays.back() + tiempoDeCola;


    for (int sample = 0; sample < numSamples; ++sample)
    {
        double t = static_cast<double>(currentSample) / getSampleRate();
        double sampleValue = 0.0;

        // Sumamos las notas con delay y envolvente
        for (size_t j = 0; j < freqs.size(); ++j)
        {
            if (t >= delays[j])
            {
                double localT = t - delays[j];

                // Solo reproducir mientras dure la envolvente
                if (localT < duration)
                {
                    for (size_t h = 0; h < harmonicsAmps.size(); ++h)
                    {
                        //double freq = freqs[j] * (h + 1);
                        double factorTraste = std::pow(2.0, static_cast<double>(trasteActual) / 12.0);
                        double freq = freqs[j] * factorTraste * (h + 1);
                        double env = envelopeGuitarra(localT);
                        sampleValue += harmonicsAmps[h] * env * sin(2.0 * pi * freq * localT);
                    }
                }
            }
        }

        // Ajuste de volumen y evitar clipping
        sampleValue *= (amplitude * Volumen / numTonos) * globalGain;
        //sample *= (amplitude / numTonos) * globalGain;

// Solo aplicar distorsión si gain > 1.0
        double normalizedSample = sampleValue / 32767.0;

        if (distortionAmount > 1.0f)
            normalizedSample *= distortionAmount;

        // Aplicar soft clipping solo si hay distorsión
        if (distortionAmount > 1.0f)
            normalizedSample = softClip(normalizedSample);


        // Limitar a rango [-1,1]
        if (sampleValue > 1.0) sampleValue = 1.0;
        if (sampleValue < -1.0) sampleValue = -1.0;

        // Escribir el sample en todos los canales
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            channelData[sample] = static_cast<float>(normalizedSample);
        }

        currentSample++;
        if (currentSample >= getSampleRate() * duration)
            currentSample = 0;  // Reiniciar ciclo del acorde

     
    }
visualizerBuffer.makeCopyOf(buffer);
}
