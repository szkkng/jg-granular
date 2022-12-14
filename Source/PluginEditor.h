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

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Dial.h"
#include "GUI/EditorLnf.h"
#include "EditorContent.h"
#include "GUI/MyColours.h"

//==============================================================================
/**
*/
class JGGranularAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JGGranularAudioProcessorEditor (JGGranularAudioProcessor& p, 
                                    juce::AudioProcessorValueTreeState& apvts,
                                    juce::UndoManager& um);
    ~JGGranularAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool keyPressed (const juce::KeyPress& key) override;

private:
    JGGranularAudioProcessor& audioProcessor;

    juce::UndoManager& undoManager;

    EditorContent editorContent;

    static constexpr int defaultWidth  { 440 };
    static constexpr int defaultHeight { 280 };

    struct SharedLnf
    {
        SharedLnf()  { juce::LookAndFeel::setDefaultLookAndFeel (&editorLnf); }
        ~SharedLnf() { juce::LookAndFeel::setDefaultLookAndFeel (nullptr); }

        EditorLnf editorLnf;
    };

    SharedResourcePointer<SharedLnf> lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JGGranularAudioProcessorEditor)
};
