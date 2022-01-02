/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Band.h"

//==============================================================================
/**
*/
class MBCompTutorialAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MBCompTutorialAudioProcessorEditor (MBCompTutorialAudioProcessor&);
    ~MBCompTutorialAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void addSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBCompTutorialAudioProcessor& audioProcessor;
    
    Band lowBand{ audioProcessor, "Low" };
    Band midBand{ audioProcessor, "Mid" };
    Band highBand{ audioProcessor, "High" };
    
    juce::Slider gainInput, gainOutput;
    
    juce::Label gainInput_Label, gainOutput_Label;
    
    juce::Slider crossoverLowMid, crossoverMidHigh;
    
    juce::Label crossoverLowMid_Label, crossoverMidHigh_Label;
    
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    sliderAttachment gainInput_Attachment, gainOutput_Attachment,
                     crossoverLowMid_Attachment, crossoverMidHigh_Attachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MBCompTutorialAudioProcessorEditor)
};
