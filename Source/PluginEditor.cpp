/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PingPongDelayAudioProcessorEditor::PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    noteButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    noteButton.addListener(this);
    noteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(233,63,0));
    addAndMakeVisible(noteButton);

    mDelayTimeLabel.setText("Tempo Sync", juce::dontSendNotification);
    mDelayTimeLabel.attachToComponent(&noteButton, true);
    mDelayTimeLabel.setColour(juce::Label::textColourId, juce::Colour(255,71,3));

    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setRange(0.1f, 0.9f, 0.01f);
    mFeedbackSlider.setValue(0.5f);
    mFeedbackSlider.setColour(juce::Slider::thumbColourId, juce::Colour(255,71,3));
    mFeedbackSlider.addListener(this);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, 1, 50, 20);
    addAndMakeVisible(mFeedbackSlider);
    
    mFeedbackLabel.setText("Feedback", juce::dontSendNotification);
    mFeedbackLabel.attachToComponent(&mFeedbackSlider, true);
    mFeedbackLabel.setColour(juce::Label::textColourId, juce::Colour(255,71,3));
    
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setRange(0.0f, 1.0f, 0.01f);
    mDryWetSlider.setValue(0.5f);
    mDryWetSlider.setColour(juce::Slider::thumbColourId, juce::Colour(255,71,3));
    mDryWetSlider.addListener(this);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextBoxRight, 1, 50, 20);
    addAndMakeVisible(mDryWetSlider);
    
    mDryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    mDryWetLabel.attachToComponent(&mDryWetSlider, true);
    mDryWetLabel.setColour(juce::Label::textColourId, juce::Colour(255,71,3));
    
    setSize (350, 300);
}

PingPongDelayAudioProcessorEditor::~PingPongDelayAudioProcessorEditor()
{
}

//==============================================================================
void PingPongDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::lightsalmon);
}

void PingPongDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    mDryWetSlider.setBounds(100, 0, 200, 100);
    mFeedbackSlider.setBounds(100, 100, 200, 100);
    noteButton.setBounds(100, 220, 150, 40);
}

void PingPongDelayAudioProcessorEditor::buttonClicked (juce::Button *button)
{
    if (button == &noteButton)
    {
        if (noteState == NoteState::eighth)
        {
            noteButton.onClick = [this]() { dottedEighth(); }; // lamda function that makes a call to dottedEighth() on button click
        }else if (noteState == NoteState::dottedEigtht)
        {
            noteButton.onClick = [this]() { quarter(); }; // lamda function that makes a call to quarter() on button click
        }else if (noteState == NoteState::quarter)
        {
            noteButton.onClick = [this]() { half(); }; // lamda function that makes a call to half() on button click
        }else if (noteState == NoteState::half)
        {
            noteButton.onClick = [this]() { eighth(); }; // lamda function that makes a call to eighth() on button click
        }
    }
}

void PingPongDelayAudioProcessorEditor::eighth()
{
    noteState = NoteState::eighth;
    audioProcessor.mNoteVal = 1;
    noteButton.setButtonText("1/8th note");
    noteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(255,85,23));
}

void PingPongDelayAudioProcessorEditor::dottedEighth()
{
    noteState = NoteState::dottedEigtht;
    audioProcessor.mNoteVal = 2;
    noteButton.setButtonText("dotted 1/8th note");
    noteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(255,113,3));
}

void PingPongDelayAudioProcessorEditor::quarter()
{
    noteState = NoteState::quarter;
    audioProcessor.mNoteVal = 3;
    noteButton.setButtonText("1/4 note");
    noteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(255,143,101));
}

void PingPongDelayAudioProcessorEditor::half()
{
    noteState = NoteState::half;
    audioProcessor.mNoteVal = 4;
    noteButton.setButtonText("1/2 note");
    noteButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(255,171,140));
}

void PingPongDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &mFeedbackSlider)
    {
        audioProcessor.mFeedback = mFeedbackSlider.getValue();
    }
    if (slider == &mDryWetSlider)
    {
       audioProcessor.mDryWet = mDryWetSlider.getValue();
    }
}
