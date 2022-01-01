/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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
    
    juce::Grid buildBand(juce::Slider& threshold, juce::Slider& attack, juce::Slider& release);
    juce::Grid buildToggles(juce::ToggleButton& bypass, juce::ToggleButton& mute, juce::ToggleButton& solo);
    
    void addRotary(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    void addToggle(juce::ToggleButton& button, const juce::String& buttonText);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MBCompTutorialAudioProcessor& audioProcessor;
        
    juce::Slider gainInput, gainOutput;
    
    juce::Label gainInput_Label, gainOutput_Label;
    
    juce::Slider thresholdL, attackL, releaseL,
                 thresholdM, attackM, releaseM,
                 thresholdH, attackH, releaseH;
    
    juce::Label  thresholdL_Label, attackL_Label, releaseL_Label,
                 thresholdM_Label, attackM_Label, releaseM_Label,
                 thresholdH_Label, attackH_Label, releaseH_Label;
    
    juce::Slider crossoverLowMid, crossoverMidHigh;
    
    juce::Label crossoverLowMid_Label, crossoverMidHigh_Label;
    
    juce::ToggleButton bypassL, muteL, soloL,
                       bypassM, muteM, soloM,
                       bypassH, muteH, soloH;
    
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    sliderAttachment gainInput_Attachment, gainOutput_Attachment,
                     thresholdL_Attachment, attackL_Attachment, releaseL_Attachment,
                     thresholdM_Attachment, attackM_Attachment, releaseM_Attachment,
                     thresholdH_Attachment, attackH_Attachment, releaseH_Attachment,
                     crossoverLowMid_Attachment, crossoverMidHigh_Attachment;
    
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    buttonAttachment bypassL_Attachment, muteL_Attachment, soloL_Attachment,
                     bypassM_Attachment, muteM_Attachment, soloM_Attachment,
                     bypassH_Attachment, muteH_Attachment, soloH_Attachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MBCompTutorialAudioProcessorEditor)
};
