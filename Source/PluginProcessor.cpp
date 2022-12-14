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
JGGranularAudioProcessor::JGGranularAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       apvts (*this, &undoManager, "Parameters", createParameterLayout())
{
    genState = (CommonState*) gen_granular::create (44100, 64);
    gen_granular::reset (genState);

    genInputBuffers  = new t_sample* [static_cast<size_t> (gen_granular::num_inputs())];
    genOutputBuffers = new t_sample* [static_cast<size_t> (gen_granular::num_outputs())];

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

            if (apvts.getParameter (name) != nullptr)
            {
                jassert (gen_granular::getparametermin (genState, i) == apvts.getParameterRange (name).start);
                jassert (gen_granular::getparametermax (genState, i) == apvts.getParameterRange (name).end);

                apvtsParamIdToGenParamIndex[name] = i;
                apvts.addParameterListener (name, this);
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
    juce::ignoreUnused (index);
}

const juce::String JGGranularAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void JGGranularAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
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
    if (layouts.getMainInputChannelSet()  == juce::AudioChannelSet::disabled()
     || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}
#endif

void JGGranularAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    for (int i = 0; i < gen_granular::num_inputs(); i++)
    {
        if (i < getTotalNumInputChannels())
        {
            for (int j = 0; j < currentBufferSize; j++)
                genInputBuffers[i][j] = buffer.getReadPointer (i)[j];
        }
        else
        {
            memset (genInputBuffers[i], 0, 
                    static_cast<size_t> (currentBufferSize) *  sizeof (double));
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
                buffer.getWritePointer (i)[j] = static_cast<float> (genOutputBuffers[i][j]);
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
    return new JGGranularAudioProcessorEditor (*this, apvts, undoManager);
    /* return new juce::GenericAudioProcessorEditor (*this); */
}

//==============================================================================
void JGGranularAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos (destData, true);
    apvts.state.writeToStream (mos);
}

void JGGranularAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, static_cast<size_t> (sizeInBytes));

    if (tree.isValid())
        apvts.replaceState (tree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JGGranularAudioProcessor();
}

void JGGranularAudioProcessor::assureBufferSize (int bufferSize)
{
    if (bufferSize > currentBufferSize)
    {
        for (int i = 0; i < gen_granular::num_inputs(); i++)
        {
            if (genInputBuffers[i])
                delete genInputBuffers[i];

            genInputBuffers[i] = new t_sample [static_cast<size_t> (bufferSize)];
        }

        for (int i = 0; i < gen_granular::num_outputs(); i++)
        {
            if (genOutputBuffers[i])
                delete genOutputBuffers[i];

            genOutputBuffers[i] = new t_sample [static_cast<size_t> (bufferSize)];
        }

        currentBufferSize = bufferSize;
    }
}

void JGGranularAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    gen_granular::setparameter (genState, 
                                apvtsParamIdToGenParamIndex[parameterID], 
                                newValue, 
                                nullptr);
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

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::mix, 1 },
                                                             ParamIDs::mix,
                                                             juce::NormalisableRange<float> (0.0f, 100.0f, 0.01f, 1.0f),
                                                             50.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::gain, 1 },
                                                             "vol",
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

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::grainPos, 1 },
                                                             "pos",
                                                             juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.405f),
                                                             100.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::grainSize, 1 },
                                                             "size",
                                                             juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.405f),
                                                             100.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::interval, 1 },
                                                             "intv",
                                                             juce::NormalisableRange<float> (10.0f, 500.0f, 0.1f, 0.405f),
                                                             100.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::pitch, 1 },
                                                             ParamIDs::pitch,
                                                             juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f, 1.0f),
                                                             0.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             [](float value, int) {
                                                                return juce::String (value, 1) + " st"; },
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::width, 1 },
                                                             ParamIDs::width,
                                                             juce::NormalisableRange<float> (0.0f, 100.0f, 0.01f, 1.0f),
                                                             50.0f,
                                                             juce::String{},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    return layout;
}
