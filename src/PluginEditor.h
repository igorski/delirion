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
#ifndef __PLUGIN_EDITOR_H_INCLUDED__
#define __PLUGIN_EDITOR_H_INCLUDED__

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

        /* automatable parameters */

        juce::Slider lfoOddControl;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoOddAttachment;

        juce::Slider lfoEvenControl;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoEvenAttachment;

        inline std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> createControl( const juce::String& title, juce::Slider& controlElement )
        {
            addAndMakeVisible( controlElement );
            controlElement.setSliderStyle ( juce::Slider::Rotary );
            controlElement.setTextBoxStyle( juce::Slider::TextBoxBelow, false, 50, 20 );

            return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( parameters, title, controlElement );
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioPluginAudioProcessorEditor )
};

#endif