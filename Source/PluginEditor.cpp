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
    addSlider(crossoverLowMid, crossoverLowMid_Label, "Low-Mid X-Over");
    addSlider(crossoverMidHigh, crossoverMidHigh_Label, "Mid-High X-Over");
    
    addSlider(gainInput, gainInput_Label, "Gain In");
    addSlider(gainOutput, gainOutput_Label, "Gain Out");
    
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
    auto bounds = getLocalBounds();
    
    auto grey = juce::Colour(48u, 54u, 61u);
    auto navy = juce::Colour(13u, 17u, 23u);
    
    g.setColour(grey); // grey for border
    g.fillAll();
    
    bounds.reduce(3, 2);
    g.setColour(navy); // navy for fill
    g.fillRoundedRectangle(bounds.toFloat(), 3);
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
    
    auto lmXOver = juce::GridItem(crossoverLowMid);
    auto mhXOver = juce::GridItem(crossoverMidHigh);
    auto inGain = juce::GridItem(gainInput);
    auto outGain = juce::GridItem(gainOutput);
    
    auto mrg = juce::GridItem::Margin(30.f, 0.f, 10.f, 0.f);
    lmXOver.margin = mrg;
    mhXOver.margin = mrg;
    inGain.margin = mrg;
    outGain.margin = mrg;
    
    xOverIO.items = {
        juce::GridItem(lmXOver),
        juce::GridItem(mhXOver),
        juce::GridItem(inGain),
        juce::GridItem(outGain)
    };
    
    xOverIO.performLayout(rightStrip);
}

void MBCompTutorialAudioProcessorEditor::addSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
}
