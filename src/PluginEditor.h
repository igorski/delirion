/*
 * Copyright (c) 2024 Igor Zinken https://www.igorski.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
    public:
        explicit AudioPluginAudioProcessorEditor( AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& state );
        ~AudioPluginAudioProcessorEditor() override;

        void paint( juce::Graphics& ) override;
        void resized() override;

    private:
        AudioPluginAudioProcessor& processorRef;
        juce::AudioProcessorValueTreeState& parameters;

        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;

        /* automatable parameters */

        juce::Slider lfoOddControl;
        juce::Slider lfoEvenControl;

        juce::Slider bitAmountControl;
        juce::Slider bitMixControl;
    
        juce::Slider lowBandControl;
        juce::Slider midBandControl;
        juce::Slider hiBandControl;
    
        juce::Slider reverbMixControl;
        juce::ToggleButton reverbFreezeControl;
    
        inline void createControl( const juce::String& title, juce::Slider& controlElement )
        {
            addAndMakeVisible( controlElement );
            controlElement.setSliderStyle ( juce::Slider::Rotary );
            controlElement.setTextBoxStyle( juce::Slider::TextBoxBelow, false, 50, 20 );

            sliderAttachments.push_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( parameters, title, controlElement )
            );
        }

        inline void createControl( const juce::String& title, juce::ToggleButton& controlElement )
        {
            addAndMakeVisible( controlElement );
            controlElement.setTitle( title );

            buttonAttachments.push_back(
                std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>( parameters, title, controlElement )
            );
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioPluginAudioProcessorEditor )
};
