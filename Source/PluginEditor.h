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
    juce::Slider thresholdRotary{ juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow },
                 attackRotary{ juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow },
                 releaseRotary{ juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    
    juce::Label thresholdLabel{ "Threshold", "Threshold" },
                attackLabel{ "Attack", "Attack" },
                releaseLabel{ "Release", "Release" };
    
    juce::ToggleButton bypassButton{ "X" },
                       muteButton{ "M" },
                       soloButton{ "S" };
    
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
        addAndMakeVisible(thresholdRotary);
        addAndMakeVisible(attackRotary);
        addAndMakeVisible(releaseRotary);
        
        addAndMakeVisible(thresholdLabel);
        addAndMakeVisible(attackLabel);
        addAndMakeVisible(releaseLabel);
        
        thresholdLabel.setJustificationType(juce::Justification::centred);
        attackLabel.setJustificationType(juce::Justification::centred);
        releaseLabel.setJustificationType(juce::Justification::centred);
        
        addAndMakeVisible(bypassButton);
        addAndMakeVisible(muteButton);
        addAndMakeVisible(soloButton);
    }
    
    // dark blue = juce::Colour(13u, 17u, 23u);
    // light grey = juce::Colour(48u, 54u, 61u);
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
        auto labelContainer = bounds.removeFromTop(10);
        
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;
        
        // Labels
        juce::Grid labels;
        labels.templateRows = { Track(Fr (1)) };
        labels.templateColumns = { Track(Fr (1)), Track(Fr (1)), Track(Fr (1)) };
        
        labels.items = { juce::GridItem(thresholdLabel), juce::GridItem(attackLabel), juce::GridItem(releaseLabel) };
        labels.performLayout(labelContainer);
        
        // Rotaries - Threshold / Attack / Release
        juce::Grid rotaries;
        rotaries.templateRows = { Track(Fr (1)) };
        rotaries.templateColumns = { Track(Fr (1)), Track(Fr (1)), Track(Fr (1)) };
        
        auto t = juce::GridItem(thresholdRotary);
        auto a = juce::GridItem(attackRotary);
        auto r = juce::GridItem(releaseRotary);
        
        int mrg = 20;
        t.margin = mrg;
        a.margin = mrg;
        r.margin = mrg;
        
        rotaries.items = { juce::GridItem(t), juce::GridItem(a), juce::GridItem(r) };
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
