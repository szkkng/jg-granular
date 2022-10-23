/*
  ==============================================================================

   Copyright 2022 Suzuki Kengo

   JG-Granular is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   JG-Granular is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with Simple Reverb. If not, see <http://www.gnu.org/licenses/>.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamIDs.h"

//==============================================================================
JGGranularAudioProcessor::JGGranularAudioProcessor() : currentBufferSize (0)
{
    genState = (CommonState*) gen_granular::create (44100, 64);
    gen_granular::reset (genState);

    genInputBuffers  = new t_sample * [gen_granular::num_inputs()];
    genOutputBuffers = new t_sample * [gen_granular::num_outputs()];

    for (int i = 0; i < gen_granular::num_inputs(); i++)
        genInputBuffers[i] = nullptr;

    for (int i = 0; i < gen_granular::num_outputs(); i++)
        genOutputBuffers[i] = nullptr;

    for (int i = 0; i < gen_granular::num_params(); ++i)
    {
        auto paramInfo = genState->params + i; 
        auto isFloatType = paramInfo->paramtype == 0;

        if (isFloatType)
        {
            auto name = juce::String (gen_granular::getparametername (genState, i));
            auto* apvtsParam = state.getParameter (name);

            if (apvtsParam != nullptr)
            {
                state.addParameterListener (name, this);

                auto range = apvtsParam->getNormalisableRange();
                jassert (gen_granular::getparametermin (genState, i) == range.start);
                jassert (gen_granular::getparametermax (genState, i) == range.end);
            }
            else
            {
                DBG (name << " doesn't exist in apvts parameters");
            }
        }
    }
}

JGGranularAudioProcessor::~JGGranularAudioProcessor()
{
    gen_granular::destroy (genState);
}

//==============================================================================
const juce::String JGGranularAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JGGranularAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JGGranularAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JGGranularAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JGGranularAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JGGranularAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JGGranularAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JGGranularAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JGGranularAudioProcessor::getProgramName (int index)
{
    return {};
}

void JGGranularAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JGGranularAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    genState->sr = sampleRate;
    genState->vs = samplesPerBlock;

    assureBufferSize (samplesPerBlock);
}

void JGGranularAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JGGranularAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void JGGranularAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    assureBufferSize (buffer.getNumSamples());

    for (int i = 0; i < gen_granular::num_inputs(); i++)
    {
        if (i < getTotalNumInputChannels())
        {
            for (int j = 0; j < currentBufferSize; j++)
                genInputBuffers[i][j] = buffer.getReadPointer (i)[j];
        }
        else
        {
            memset (genInputBuffers[i], 0, currentBufferSize *  sizeof (double));
        }
    }

    gen_granular::perform (genState,
                           genInputBuffers,
                           gen_granular::num_inputs(),
                           genOutputBuffers,
                           gen_granular::num_outputs(),
                           buffer.getNumSamples());

    for (int i = 0; i < getTotalNumOutputChannels(); i++)
    {
        if (i < gen_granular::num_outputs())
        {
            for (int j = 0; j < buffer.getNumSamples(); j++)
                buffer.getWritePointer (i)[j] = genOutputBuffers[i][j];
        }
        else
        {
            buffer.clear (i, 0, buffer.getNumSamples());
        }
    }
}

//==============================================================================
bool JGGranularAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JGGranularAudioProcessor::createEditor()
{
    return new JGGranularAudioProcessorEditor (*this, state, undoManager);
    /* return new juce::GenericAudioProcessorEditor (*this); */
}

//==============================================================================
void JGGranularAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos (destData, true);
    state.state.writeToStream (mos);
}

void JGGranularAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, sizeInBytes);

    if (tree.isValid())
        state.replaceState (tree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JGGranularAudioProcessor();
}

void JGGranularAudioProcessor::assureBufferSize (long bufferSize)
{
    if (bufferSize > currentBufferSize)
    {
        for (int i = 0; i < gen_granular::num_inputs(); i++)
        {
            if (genInputBuffers[i])
                delete genInputBuffers[i];

            genInputBuffers[i] = new t_sample [bufferSize];
        }

        for (int i = 0; i < gen_granular::num_outputs(); i++)
        {
            if (genOutputBuffers[i])
                delete genOutputBuffers[i];

            genOutputBuffers[i] = new t_sample [bufferSize];
        }

        currentBufferSize = bufferSize;
    }
}

void JGGranularAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    for (int i = 0; i < gen_granular::num_params(); ++i)
    {
        auto paramName = gen_granular::getparametername (genState, i);

        if (parameterID == paramName)
        {
            gen_granular::setparameter (genState, i, newValue, nullptr);
            return;
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout JGGranularAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto percentFormat = [](float value, int)
    {
        if (value < 10.0f)
            return juce::String (value, 2) + " %";
        else if (value < 100.0f)
            return juce::String (value, 1) + " %";
        else
            return juce::String (value, 0) + " %";
    };

    auto msFormat = [](float value, int)
    {
        if (value < 100.0f)
            return juce::String (value, 1) + " ms";
        else
            return juce::String (std::roundf (value)) + " ms";
    };

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::mix,
                                                             ParamIDs::mix,
                                                             juce::NormalisableRange<float> (0.0f, 100.0f, 0.01f, 1.0f),
                                                             50.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::gain,
                                                             ParamIDs::gain,
                                                             juce::NormalisableRange<float> (-36.0, 12.0, 0.1f, 2.4f),
                                                             0.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             [](float value, int) {
                                                             if (value <= -10.0f)
                                                                return juce::String (std::roundf (value)) + " dB";
                                                             else
                                                                return juce::String (value, 1) + " dB"; },
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::grainPos,
                                                             ParamIDs::grainPos,
                                                             juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.405f),
                                                             100.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::grainSize,
                                                             ParamIDs::grainSize,
                                                             juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.405f),
                                                             100.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::interval,
                                                             ParamIDs::interval,
                                                             juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.405f),
                                                             100.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::pitch,
                                                             ParamIDs::pitch,
                                                             juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f, 1.0f),
                                                             0.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             [](float value, int) {
                                                                return juce::String (value, 1) + " st"; },
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::width,
                                                             ParamIDs::width,
                                                             juce::NormalisableRange<float> (0.0f, 100.0f, 0.01f, 1.0f),
                                                             50.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    return layout;
}
