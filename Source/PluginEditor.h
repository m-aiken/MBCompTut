/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct Band : juce::Component
{
    juce::Slider thresholdRotary, attackRotary, releaseRotary;
    juce::Label thresholdLabel, attackLabel, releaseLabel;
    juce::Label bandLabel;
    juce::ToggleButton bypassButton{ "X" }, muteButton{ "M" }, soloButton{ "S" };
    
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    sliderAttachment thresholdAttachment, attackAttachment, releaseAttachment;
    
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    buttonAttachment bypassAttachment, muteAttachment, soloAttachment;
    
    Band(MBCompTutorialAudioProcessor& p, const juce::String& bandName) :
        thresholdAttachment(p.apvts, ("Threshold " + bandName + " Band"), thresholdRotary),
        attackAttachment(p.apvts, ("Attack " + bandName + " Band"), attackRotary),
        releaseAttachment(p.apvts, ("Release " + bandName + " Band"), releaseRotary),
        bypassAttachment(p.apvts, ("Bypass " + bandName + " Band"), bypassButton),
        muteAttachment(p.apvts, ("Mute " + bandName + " Band"), muteButton),
        soloAttachment(p.apvts, ("Solo " + bandName + " Band"), soloButton)
    {
        addRotary(thresholdRotary, thresholdLabel, "Threshold");
        addRotary(attackRotary, attackLabel, "Attack");
        addRotary(releaseRotary, releaseLabel, "Release");
        
        addAndMakeVisible(bypassButton);
        addAndMakeVisible(muteButton);
        addAndMakeVisible(soloButton);
        
        addAndMakeVisible(bandLabel);
        bandLabel.setText(bandName, juce::dontSendNotification);
        bandLabel.setJustificationType(juce::Justification::centred);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        
        auto grey = juce::Colour(48u, 54u, 61u);
        auto navy = juce::Colour(13u, 17u, 23u);
        
        g.setColour(grey); // grey for border
        g.fillAll();
        
        bounds.reduce(3, 2);
        g.setColour(navy); // navy for fill
        g.fillRoundedRectangle(bounds.toFloat(), 3);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto toggleContainer = bounds.removeFromRight(50);
        
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;
        
        // Rotaries - Threshold / Attack / Release
        juce::Grid rotaries;
        rotaries.templateRows = { Track(Fr (1)) };
        rotaries.templateColumns = { Track(Fr (3)), Track(Fr (5)), Track(Fr (5)), Track(Fr (5)) };
        
        auto tRotary = juce::GridItem(thresholdRotary);
        auto aRotary = juce::GridItem(attackRotary);
        auto rRotary = juce::GridItem(releaseRotary);
        
        // margin args = top, right, bottom, left
        auto rotaryMargin = juce::GridItem::Margin(30.f, 0.f, 10.f, 0.f);
        tRotary.margin = rotaryMargin;
        aRotary.margin = rotaryMargin;
        rRotary.margin = rotaryMargin;
        
        rotaries.items = { juce::GridItem(bandLabel), juce::GridItem(tRotary), juce::GridItem(aRotary), juce::GridItem(rRotary) };
        rotaries.performLayout(bounds);
        
        // Toggle buttons
        juce::Grid toggles;
        toggles.templateRows = { Track(Fr (1)), Track(Fr (1)), Track(Fr (1)) };
        toggles.templateColumns = { Track(Fr (1)) };
        
        toggles.items = {
            juce::GridItem(bypassButton),
            juce::GridItem(muteButton),
            juce::GridItem(soloButton)
        };
        
        toggles.performLayout(toggleContainer);
    }
    
    void addRotary(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
        
        addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.attachToComponent(&slider, false);
    }
};

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
