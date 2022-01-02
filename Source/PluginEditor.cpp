/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MBCompTutorialAudioProcessorEditor::MBCompTutorialAudioProcessorEditor (MBCompTutorialAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      gainInput_Attachment(audioProcessor.apvts, "Gain In", gainInput),
      gainOutput_Attachment(audioProcessor.apvts, "Gain Out", gainOutput),
      crossoverLowMid_Attachment(audioProcessor.apvts, "Low-Mid Crossover Frequency", crossoverLowMid),
      crossoverMidHigh_Attachment(audioProcessor.apvts, "Mid-High Crossover Frequency", crossoverMidHigh)
{
    addRotary(crossoverLowMid, crossoverLowMid_Label, "Low-Mid Crossover");
    addRotary(crossoverMidHigh, crossoverMidHigh_Label, "Mid-High Crossover");
    
    addRotary(gainInput, gainInput_Label, "Gain In");
    addRotary(gainOutput, gainOutput_Label, "Gain Out");
    
    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);
    
    setSize (600, 450);
}

MBCompTutorialAudioProcessorEditor::~MBCompTutorialAudioProcessorEditor()
{
}

//==============================================================================
void MBCompTutorialAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void MBCompTutorialAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto rightStrip = bounds.removeFromRight(150);
    
    lowBand.setBounds(bounds.removeFromTop(150));
    midBand.setBounds(bounds.removeFromTop(150));
    highBand.setBounds(bounds);

    // Crossover / IO Gain
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    
    juce::Grid xOverIO;
    xOverIO.templateRows = { Track(Fr (1)), Track(Fr (1)), Track(Fr (1)), Track(Fr (1)) };
    xOverIO.templateColumns = { Track(Fr (1)) };
    
    auto lmX = juce::GridItem(crossoverLowMid);
    auto mhX = juce::GridItem(crossoverMidHigh);
    auto inG = juce::GridItem(gainInput);
    auto outG = juce::GridItem(gainOutput);
    
    int mrg = 20;
    lmX.margin = mrg;
    mhX.margin = mrg;
    inG.margin = mrg;
    outG.margin = mrg;
    
    xOverIO.items = {
        juce::GridItem(lmX),
        juce::GridItem(mhX),
        juce::GridItem(inG),
        juce::GridItem(outG)
    };
    
    xOverIO.performLayout(rightStrip);
}

void MBCompTutorialAudioProcessorEditor::addRotary(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
}
