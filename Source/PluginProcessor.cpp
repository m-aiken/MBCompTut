/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBCompTutorialAudioProcessor::MBCompTutorialAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // apvts's getParameter function returns a RangedAudioParameter pointer
    // cast that to the correct type ie. AudioParameterFloat pointer
    // getParameter returns null if the provided parameter ID (string) is not found
    // jassert as safety check - will also catch if cast fails
    const auto& params = Params::getParams();
    
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    
    floatHelper(compressor.threshold, Params::Names::Threshold_Low_Band);
    floatHelper(compressor.attack, Params::Names::Attack_Low_Band);
    floatHelper(compressor.release, Params::Names::Release_Low_Band);
    choiceHelper(compressor.ratio, Params::Names::Ratio_Low_Band);
    boolHelper(compressor.bypass, Params::Names::Bypass_Low_Band);
    floatHelper(lowMidCrossover, Params::Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Params::Names::Mid_High_Crossover_Freq);
    
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

MBCompTutorialAudioProcessor::~MBCompTutorialAudioProcessor()
{
}

//==============================================================================
const juce::String MBCompTutorialAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MBCompTutorialAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MBCompTutorialAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MBCompTutorialAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MBCompTutorialAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MBCompTutorialAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MBCompTutorialAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MBCompTutorialAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MBCompTutorialAudioProcessor::getProgramName (int index)
{
    return {};
}

void MBCompTutorialAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MBCompTutorialAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    compressor.prepare(spec);
    
    LP1.prepare(spec);
    HP1.prepare(spec);
    AP2.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    
    for ( auto& buffer : filterBuffers )
    {
        buffer.setSize(spec.numChannels, spec.maximumBlockSize);
    }
}

void MBCompTutorialAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MBCompTutorialAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MBCompTutorialAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
//    compressor.updateCompressorSettings();
//    compressor.process(buffer);
    
    for ( auto& fb : filterBuffers )
    {
        fb = buffer;
    }
        
    auto lowMidCutoff = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoff);
    HP1.setCutoffFrequency(lowMidCutoff);
    
    auto midHighCutoff = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoff);
    LP2.setCutoffFrequency(midHighCutoff);
    HP2.setCutoffFrequency(midHighCutoff);
    
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
    
    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);
    
    LP1.process(fb0Ctx);
    AP2.process(fb0Ctx);
    
    HP1.process(fb1Ctx);
    filterBuffers[2] = filterBuffers[1];
    LP2.process(fb1Ctx);
    
    HP2.process(fb2Ctx);
    
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    if ( compressor.bypass->get() )
        return;
    
    buffer.clear();
    
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for ( auto i = 0; i < nc; ++i )
        {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };
    
    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);
    addFilterBand(buffer, filterBuffers[2]);
}

//==============================================================================
bool MBCompTutorialAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MBCompTutorialAudioProcessor::createEditor()
{
//    return new MBCompTutorialAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void MBCompTutorialAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream (destData, true);
    apvts.state.writeToStream(stream);
}

void MBCompTutorialAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid() )
    {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout MBCompTutorialAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    const auto& params = Params::getParams();
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Params::Names::Threshold_Low_Band),
                                                           params.at(Params::Names::Threshold_Low_Band),
                                                           juce::NormalisableRange<float>(-60.f, 12.f, 1.f, 1.f),
                                                           0.f));
    
    auto attackReleaseRange = juce::NormalisableRange<float>(5.f, 500.f, 1.f, 1.f);
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Params::Names::Attack_Low_Band),
                                                           params.at(Params::Names::Attack_Low_Band),
                                                           attackReleaseRange,
                                                           50.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Params::Names::Release_Low_Band),
                                                           params.at(Params::Names::Release_Low_Band),
                                                           attackReleaseRange,
                                                           250.f));
    
    //       Ratio
    //   1:1 = no compression
    // 100:1 = brick wall compression
    
    auto choices = std::vector<double>{ 1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20, 50, 100 };
    juce::StringArray ratioChoices;
    for ( auto choice : choices )
    {
        ratioChoices.add( juce::String(choice, 1) );
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(Params::Names::Ratio_Low_Band),
                                                            params.at(Params::Names::Ratio_Low_Band),
                                                            ratioChoices,
                                                            3));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Params::Names::Bypass_Low_Band),
                                                          params.at(Params::Names::Bypass_Low_Band),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Params::Names::Low_Mid_Crossover_Freq),
                                                           params.at(Params::Names::Low_Mid_Crossover_Freq),
                                                           juce::NormalisableRange<float>(20.f, 999.f, 1.f, 1.f),
                                                           400.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Params::Names::Mid_High_Crossover_Freq),
                                                           params.at(Params::Names::Mid_High_Crossover_Freq),
                                                           juce::NormalisableRange<float>(1000.f, 20000.f, 1.f, 1.f),
                                                           2000.f));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MBCompTutorialAudioProcessor();
}
