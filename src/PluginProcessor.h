/*
 * Copyright( c) 2024 Igor Zinken https://www.igorski.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *( at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __PLUGIN_PROCESSOR_H_INCLUDED__
#define __PLUGIN_PROCESSOR_H_INCLUDED__

#include <juce_audio_processors/juce_audio_processors.h>
#include "DopplerEffect.h"
#include "Parameters.h"
#include "ParameterListener.h"
#include "ParameterSubscriber.h"

/* TODO: not in release build */
#include "utils/Debug.h"

class AudioPluginAudioProcessor final : public juce::AudioProcessor, ParameterSubscriber
{
    public:
        static const juce::String pluginUUID; // @todo verify

        AudioPluginAudioProcessor();
        ~AudioPluginAudioProcessor() override;

        /* configuration */

        const juce::String getName() const override;
        bool isBusesLayoutSupported( const BusesLayout& layouts ) const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        /* programs */

        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram( int index ) override;
        const juce::String getProgramName( int index ) override;
        void changeProgramName( int index, const juce::String& newName ) override;

        /* resource management */

        void prepareToPlay( double sampleRate, int samplesPerBlock ) override;
        void releaseResources() override;

        /* rendering */

        void processBlock( juce::AudioBuffer<float>&, juce::MidiBuffer& ) override;
        using AudioProcessor::processBlock;

        /* automatable parameters */

        juce::AudioProcessorValueTreeState parameters;
        ParameterListener parameterListener;
        void updateParameters() override;

        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
        {
            std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::LFO_ODD,  "LFO odd",  0.f, 1.f, 0.01f ));
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::LFO_EVEN, "LFO even", 0.f, 1.f, 0.05f ));
            
            return { params.begin(), params.end() };
        }

        /* editor */
        
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        /* persistence */

        void getStateInformation( juce::MemoryBlock& destData ) override;
        void setStateInformation( const void* data, int sizeInBytes ) override;

    private:
        juce::OwnedArray<juce::IIRFilter> lowPassFilters;
        juce::OwnedArray<juce::IIRFilter> bandPassFilters;
        juce::OwnedArray<juce::IIRFilter> highPassFilters;

        juce::OwnedArray<DopplerEffect> dopplerEffects;

        Debug debug; // debug only

        // parameters

        std::atomic<float>* lfoOdd;
        std::atomic<float>* lfoEven;
        
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioPluginAudioProcessor )
};

// @todo
// const juce::String AudioPluginAudioProcessor::pluginUUID = "12345678-1234-1234-1234-123456789abc";

#endif