/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#define MAX_DELAY_TIME 2

//==============================================================================
/**
*/
class PingPongDelayAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Slider::Listener,
                                           public juce::Button::Listener
{
public:
    PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor&);
    ~PingPongDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;
    void buttonClicked (juce::Button *button) override;
    void eighth();
    void dottedEighth();
    void quarter();
    void half();
    

private:
    
    juce::Slider mFeedbackSlider;
    juce::Slider mDryWetSlider;
    
    juce::Label mDryWetLabel;
    juce::Label mFeedbackLabel;
    juce::Label mDelayTimeLabel;
    
    enum class NoteState
    {
        eighth,
        dottedEigtht,
        quarter,
        half
    };
    
    NoteState noteState { NoteState::eighth };
    juce::TextButton noteButton { "1/8th note" };
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PingPongDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongDelayAudioProcessorEditor)
};
