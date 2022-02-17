/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include<math.h>

//==============================================================================
PingPongDelayAudioProcessor::PingPongDelayAudioProcessor()
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
}

PingPongDelayAudioProcessor::~PingPongDelayAudioProcessor()
{
    if (mLeftDelayLine != NULL)
    {
        delete [] mLeftDelayLine;
    }
    if (mRightDelayLine != NULL)
    {
        delete [] mRightDelayLine;
    }
}

//==============================================================================
const juce::String PingPongDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PingPongDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PingPongDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PingPongDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PingPongDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PingPongDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PingPongDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PingPongDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PingPongDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void PingPongDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PingPongDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    //check buffers are nullptrs
    if (mLeftDelayLine != NULL)
    {
        delete [] mLeftDelayLine;
    }
    if (mRightDelayLine != NULL)
    {
        delete [] mRightDelayLine;
    }
    
    mBufferSize = getSampleRate() * MAX_DELAY_TIME;
    
    //allocate memory for buffers
    mLeftDelayLine = new float[mBufferSize];
    mRightDelayLine = new float[mBufferSize];
    
    //flush buffers
    for(int n = 0; n < mBufferSize; n++)
    {
        mLeftDelayLine[n] = 0.0f;
        mRightDelayLine[n] = 0.0f;
    }
    
    //initialise read and write index
    mReadHead = 0;
    mWriteHead = mDelayInSamples;
    
}

void PingPongDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PingPongDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void PingPongDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    int numSamples = buffer.getNumSamples();
    
    if(!numSamples)
    {
        return;
    }
    
//  mDelayInSamples = mDelayInSeconds * getSampleRate();
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    
    auto leftChannelData = buffer.getWritePointer(0);
    auto rightChannelData = buffer.getWritePointer(0);
    
    if (totalNumInputChannels == 2)
    {
        rightChannelData = buffer.getWritePointer(1);
    }
    
    float ipL = 0.0f; //left input sample
    float ipR = 0.0f; //right input sample
    
    //tempo sync
    if (auto* PlayHead = getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo newTime;
        if (PlayHead->getCurrentPosition (newTime))
        {
            beatsPerMinute = newTime.bpm;
            beatsPerSecond = beatsPerMinute/60.0f;
    
            //find seconds per beat by taking the inverse
            secondsPerBeat = 1.0f/beatsPerSecond;
            switch (mNoteVal) {
                case 1:
                    noteLength = 0.5f; // 1/8th note delay
                    break;
                case 2:
                    noteLength = 0.75f; // dotted 1/8th note delay
                    break;
                case 3:
                    noteLength = 1.0f; // 1/4 note delay
                    break;
                case 4:
                    noteLength = 2.0f; // 1/2 note delay
                    break;
                default: noteLength = 1.0f;
                    break;
            }
            mDelayInSamples = (int)(noteLength * secondsPerBeat * getSampleRate());
        }
    }else
    {
        mDelayInSamples = 1 * getSampleRate();
    }
    
    //Begin main time loop
    for(int n = 0; n < numSamples; n++)
    {
        //Read samples in from input
        ipL = buffer.getSample(0, n);
        ipR = buffer.getSample(1, n);
        
        //wrap circular buffer
        mReadHead %= mDelayInSamples;
        mWriteHead %= mDelayInSamples;

        //Get the current output of delay ring buffer
        float delayOutLeft = mLeftDelayLine[mReadHead];
        float delayOutRight = mRightDelayLine[mReadHead];
        
        //implement ping pong effect 
        float delayInLeft = ipR + mFeedback * delayOutRight;
        float delayInRight = ipL + mFeedback * delayOutLeft;
        
        mLeftDelayLine[mWriteHead] = delayInLeft;
        mRightDelayLine[mWriteHead] = delayInRight;
        
        leftChannelData[n] = mDryWet * ipL + (1.0f - mDryWet) * delayOutLeft;
        rightChannelData[n] = mDryWet * ipR + (1.0f - mDryWet) * delayOutRight;
        
        mReadHead++;
        mWriteHead++;
    }
    
}

//==============================================================================
bool PingPongDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PingPongDelayAudioProcessor::createEditor()
{
    return new PingPongDelayAudioProcessorEditor (*this);
}

//==============================================================================
void PingPongDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PingPongDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PingPongDelayAudioProcessor();
}
