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
      thresholdL_Attachment(audioProcessor.apvts, "Threshold Low Band", thresholdL),
      attackL_Attachment(audioProcessor.apvts, "Attack Low Band", attackL),
      releaseL_Attachment(audioProcessor.apvts, "Release Low Band", releaseL),
      thresholdM_Attachment(audioProcessor.apvts, "Threshold Mid Band", thresholdM),
      attackM_Attachment(audioProcessor.apvts, "Attack Mid Band", attackM),
      releaseM_Attachment(audioProcessor.apvts, "Release Mid Band", releaseM),
      thresholdH_Attachment(audioProcessor.apvts, "Threshold High Band", thresholdH),
      attackH_Attachment(audioProcessor.apvts, "Attack High Band", attackH),
      releaseH_Attachment(audioProcessor.apvts, "Release High Band", releaseH),
      crossoverLowMid_Attachment(audioProcessor.apvts, "Low-Mid Crossover Frequency", crossoverLowMid),
      crossoverMidHigh_Attachment(audioProcessor.apvts, "Mid-High Crossover Frequency", crossoverMidHigh),
      bypassL_Attachment(audioProcessor.apvts, "Bypass Low Band", bypassL),
      muteL_Attachment(audioProcessor.apvts, "Mute Low Band", muteL),
      soloL_Attachment(audioProcessor.apvts, "Solo Low Band", soloL),
      bypassM_Attachment(audioProcessor.apvts, "Bypass Mid Band", bypassM),
      muteM_Attachment(audioProcessor.apvts, "Mute Mid Band", muteM),
      soloM_Attachment(audioProcessor.apvts, "Solo Mid Band", soloM),
      bypassH_Attachment(audioProcessor.apvts, "Bypass High Band", bypassH),
      muteH_Attachment(audioProcessor.apvts, "Mute High Band", muteH),
      soloH_Attachment(audioProcessor.apvts, "Solo High Band", soloH)
{
    addRotary(thresholdL, thresholdL_Label, "Threshold");
    addRotary(thresholdM, thresholdM_Label, "Threshold");
    addRotary(thresholdH, thresholdH_Label, "Threshold");
    
    addRotary(attackL, attackL_Label, "Attack");
    addRotary(attackM, attackM_Label, "Attack");
    addRotary(attackH, attackH_Label, "Attack");
    
    addRotary(releaseL, releaseL_Label, "Release");
    addRotary(releaseM, releaseM_Label, "Release");
    addRotary(releaseH, releaseH_Label, "Release");
    
    addRotary(crossoverLowMid, crossoverLowMid_Label, "Low-Mid Crossover");
    addRotary(crossoverMidHigh, crossoverMidHigh_Label, "Mid-High Crossover");
    
    addRotary(gainInput, gainInput_Label, "Gain In");
    addRotary(gainOutput, gainOutput_Label, "Gain Out");
    
    addToggle(bypassL, "X");
    addToggle(bypassM, "X");
    addToggle(bypassH, "X");
    
    addToggle(muteL, "M");
    addToggle(muteM, "M");
    addToggle(muteH, "M");
    
    addToggle(soloL, "S");
    addToggle(soloM, "S");
    addToggle(soloH, "S");
    
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
  
    auto low = bounds.removeFromTop(150);
    auto lowToggles = low.removeFromRight(50);
    
    auto mid = bounds.removeFromTop(150);
    auto midToggles = mid.removeFromRight(50);
    
    auto high = bounds.removeFromTop(150);
    auto highToggles = high.removeFromRight(50);
    
    // Low band
    auto lowControls = buildBand(thresholdL, attackL, releaseL);
    lowControls.performLayout(low);
    auto lToggles = buildToggles(bypassL, muteL, soloL);
    lToggles.performLayout(lowToggles);
    // Mid band
    auto midControls = buildBand(thresholdM, attackM, releaseM);
    midControls.performLayout(mid);
    auto mToggles = buildToggles(bypassM, muteM, soloM);
    mToggles.performLayout(midToggles);
    // High band
    auto highControls = buildBand(thresholdH, attackH, releaseH);
    highControls.performLayout(high);
    auto hToggles = buildToggles(bypassH, muteH, soloH);
    hToggles.performLayout(highToggles);

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

juce::Grid MBCompTutorialAudioProcessorEditor::buildBand(juce::Slider& threshold, juce::Slider& attack, juce::Slider& release)
{
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    
    juce::Grid band;
    band.templateRows = { Track(Fr (1)) };
    band.templateColumns = { Track(Fr (1)), Track(Fr (1)), Track(Fr (1)) };
    
    auto t = juce::GridItem(threshold);
    auto a = juce::GridItem(attack);
    auto r = juce::GridItem(release);
    
    int mrg = 20;
    t.margin = mrg;
    a.margin = mrg;
    r.margin = mrg;
    
    band.items = { juce::GridItem(t), juce::GridItem(a), juce::GridItem(r) };
    return band;
}

juce::Grid MBCompTutorialAudioProcessorEditor::buildToggles(juce::ToggleButton& bypass, juce::ToggleButton& mute, juce::ToggleButton& solo)
{
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    juce::Grid toggles;
    toggles.templateRows = { Track(Fr (1)), Track(Fr (1)), Track(Fr (1)) };
    toggles.templateColumns = { Track(Fr (1)) };
    
    toggles.items = {
        juce::GridItem(bypass),
        juce::GridItem(mute),
        juce::GridItem(solo)
    };
    
    return toggles;
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

void MBCompTutorialAudioProcessorEditor::addToggle(juce::ToggleButton& button, const juce::String& buttonText)
{
    addAndMakeVisible(button);
    button.setButtonText(buttonText);
}
