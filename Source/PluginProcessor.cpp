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
    
    floatHelper(lowBandComp.threshold, Params::Names::Threshold_Low_Band);
    floatHelper(lowBandComp.attack, Params::Names::Attack_Low_Band);
    floatHelper(lowBandComp.release, Params::Names::Release_Low_Band);
    choiceHelper(lowBandComp.ratio, Params::Names::Ratio_Low_Band);
    boolHelper(lowBandComp.bypass, Params::Names::Bypass_Low_Band);
    boolHelper(lowBandComp.mute, Params::Names::Mute_Low_Band);
    boolHelper(lowBandComp.solo, Params::Names::Solo_Low_Band);
    
    floatHelper(midBandComp.threshold, Params::Names::Threshold_Mid_Band);
    floatHelper(midBandComp.attack, Params::Names::Attack_Mid_Band);
    floatHelper(midBandComp.release, Params::Names::Release_Mid_Band);
    choiceHelper(midBandComp.ratio, Params::Names::Ratio_Mid_Band);
    boolHelper(midBandComp.bypass, Params::Names::Bypass_Mid_Band);
    boolHelper(midBandComp.mute, Params::Names::Mute_Mid_Band);
    boolHelper(midBandComp.solo, Params::Names::Solo_Mid_Band);
    
    floatHelper(highBandComp.threshold, Params::Names::Threshold_High_Band);
    floatHelper(highBandComp.attack, Params::Names::Attack_High_Band);
    floatHelper(highBandComp.release, Params::Names::Release_High_Band);
    choiceHelper(highBandComp.ratio, Params::Names::Ratio_High_Band);
    boolHelper(highBandComp.bypass, Params::Names::Bypass_High_Band);
    boolHelper(highBandComp.mute, Params::Names::Mute_High_Band);
    boolHelper(highBandComp.solo, Params::Names::Solo_High_Band);
    
    floatHelper(lowMidCrossover, Params::Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Params::Names::Mid_High_Crossover_Freq);
    
    floatHelper(inputGainParam, Params::Names::Gain_In);
    floatHelper(outputGainParam, Params::Names::Gain_Out);
    
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
    
    for ( auto& compressor : compressors )
        compressor.prepare(spec);
    
    LP1.prepare(spec);
    HP1.prepare(spec);
    AP2.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    
    inputGain.prepare(spec);
    outputGain.prepare(spec);
    
    inputGain.setRampDurationSeconds(0.05);
    outputGain.setRampDurationSeconds(0.05);
    
    for ( auto& buffer : filterBuffers )
        buffer.setSize(spec.numChannels, spec.maximumBlockSize);
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

void MBCompTutorialAudioProcessor::updateState()
{
    for ( auto& compressor : compressors)
        compressor.updateCompressorSettings();
    
    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());
    
    auto lowMidCutoff = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoff);
    HP1.setCutoffFrequency(lowMidCutoff);
    
    auto midHighCutoff = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoff);
    LP2.setCutoffFrequency(midHighCutoff);
    HP2.setCutoffFrequency(midHighCutoff);
}

void MBCompTutorialAudioProcessor::splitBands(const juce::AudioBuffer<float> &inputBuffer)
{
    for ( auto& fb : filterBuffers )
        fb = inputBuffer;
    
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
}

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
   
    updateState();
    
    applyGain(buffer, inputGain);
        
    splitBands(buffer);
    
    for ( size_t i = 0; i < filterBuffers.size(); ++i )
        compressors[i].process(filterBuffers[i]);
    
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    buffer.clear();
    
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for ( auto i = 0; i < nc; ++i )
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
    };
    
    auto bandSolod = false;
    for ( auto& comp : compressors )
    {
        if ( comp.solo->get() )
        {
            bandSolod = true;
            break;
        }
    }
    
    if ( bandSolod )
    {
        for ( size_t i = 0; i < compressors.size(); ++i )
        {
            auto& comp = compressors[i];
            if ( comp.solo->get() )
            {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }
    else
    {
        for ( size_t i = 0; i < compressors.size(); ++i )
        {
            auto& comp = compressors[i];
            if ( !comp.mute->get() )
            {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }
    
    applyGain(buffer, outputGain);
}

//==============================================================================
bool MBCompTutorialAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MBCompTutorialAudioProcessor::createEditor()
{
    return new MBCompTutorialAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
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

void MBCompTutorialAudioProcessor::addFloatParam(APVTS::ParameterLayout& layout, const juce::String& name, const juce::NormalisableRange<float>& range, const float defaultVal)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(name, name, range, defaultVal));
}

void MBCompTutorialAudioProcessor::addBoolParam(APVTS::ParameterLayout& layout, const juce::String& name, const bool defaultVal)
{
    layout.add(std::make_unique<juce::AudioParameterBool>(name, name, defaultVal));
}

juce::AudioProcessorValueTreeState::ParameterLayout MBCompTutorialAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    const auto& params = Params::getParams();
    
    // I/O Gain
    auto gainRange = juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f);
    addFloatParam(layout, params.at(Params::Names::Gain_In), gainRange, 0.f);
    addFloatParam(layout, params.at(Params::Names::Gain_Out), gainRange, 0.f);
    
    // Threshold
    auto thresholdRange = juce::NormalisableRange<float>(-60.f, 12.f, 1.f, 1.f);
    addFloatParam(layout, params.at(Params::Names::Threshold_Low_Band), thresholdRange, 0.f);
    addFloatParam(layout, params.at(Params::Names::Threshold_Mid_Band), thresholdRange, 0.f);
    addFloatParam(layout, params.at(Params::Names::Threshold_High_Band), thresholdRange, 0.f);
    
    // Attack/Release
    auto attackReleaseRange = juce::NormalisableRange<float>(5.f, 500.f, 1.f, 1.f);
    addFloatParam(layout, params.at(Params::Names::Attack_Low_Band), attackReleaseRange, 50.f);
    addFloatParam(layout, params.at(Params::Names::Attack_Mid_Band), attackReleaseRange, 50.f);
    addFloatParam(layout, params.at(Params::Names::Attack_High_Band), attackReleaseRange, 50.f);
    
    addFloatParam(layout, params.at(Params::Names::Release_Low_Band), attackReleaseRange, 250.f);
    addFloatParam(layout, params.at(Params::Names::Release_Mid_Band), attackReleaseRange, 250.f);
    addFloatParam(layout, params.at(Params::Names::Release_High_Band), attackReleaseRange, 250.f);
    
    //       Ratio
    //   1:1 = no compression
    // 100:1 = brick wall compression
    
    auto choices = std::vector<double>{ 1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20, 50, 100 };
    juce::StringArray ratioChoices;
    for ( auto choice : choices )
    {
        ratioChoices.add( juce::String(choice, 1) );
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(Params::Names::Ratio_Low_Band), params.at(Params::Names::Ratio_Low_Band), ratioChoices, 3));
    layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(Params::Names::Ratio_Mid_Band), params.at(Params::Names::Ratio_Mid_Band), ratioChoices, 3));
    layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(Params::Names::Ratio_High_Band), params.at(Params::Names::Ratio_High_Band), ratioChoices, 3));
    
    // Bypass
    addBoolParam(layout, params.at(Params::Names::Bypass_Low_Band), false);
    addBoolParam(layout, params.at(Params::Names::Bypass_Mid_Band), false);
    addBoolParam(layout, params.at(Params::Names::Bypass_High_Band), false);

    // Mute
    addBoolParam(layout, params.at(Params::Names::Mute_Low_Band), false);
    addBoolParam(layout, params.at(Params::Names::Mute_Mid_Band), false);
    addBoolParam(layout, params.at(Params::Names::Mute_High_Band), false);

    // Solo
    addBoolParam(layout, params.at(Params::Names::Solo_Low_Band), false);
    addBoolParam(layout, params.at(Params::Names::Solo_Mid_Band), false);
    addBoolParam(layout, params.at(Params::Names::Solo_High_Band), false);

    // Crossover
    addFloatParam(layout, params.at(Params::Names::Low_Mid_Crossover_Freq), juce::NormalisableRange<float>(20.f, 999.f, 1.f, 1.f), 400.f);
    addFloatParam(layout, params.at(Params::Names::Mid_High_Crossover_Freq), juce::NormalisableRange<float>(1000.f, 20000.f, 1.f, 1.f), 2000.f);
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MBCompTutorialAudioProcessor();
}
