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

#include <juce_audio_processors/juce_audio_processors.h>
// #include "modules/bitcrusher/Bitcrusher.h"
#include "modules/doppler/DopplerEffect.h"
#include "modules/reverb/Reverb.h"
#include "modules/waveshaper/WaveShaper.h"
#include "Parameters.h"
#include "ParameterListener.h"
#include "ParameterSubscriber.h"

class AudioPluginAudioProcessor final : public juce::AudioProcessor, ParameterSubscriber
{
    public:
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

            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::LOW_LFO_ODD,  "Low LFO odd",  0.f, 1.f, 0.f ));
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::LOW_LFO_EVEN, "Low LFO even", 0.f, 1.f, 0.f ));
            params.push_back( std::make_unique<juce::AudioParameterBool> ( Parameters::LOW_LFO_LINK, "Low LFO link", true )); 
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::MID_LFO_ODD,  "Mid LFO odd",  0.f, 1.f, 0.01f ));
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::MID_LFO_EVEN, "Mid LFO even", 0.f, 1.f, 0.05f ));
            params.push_back( std::make_unique<juce::AudioParameterBool> ( Parameters::MID_LFO_LINK, "Mid LFO link", true )); 
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::HI_LFO_ODD,   "Hi LFO odd",   0.f, 1.f, 0.f ));
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::HI_LFO_EVEN,  "Hi LFO even",  0.f, 1.f, 0.f ));
            params.push_back( std::make_unique<juce::AudioParameterBool> ( Parameters::HI_LFO_LINK,  "Hi LFO link",  true )); 
            
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::DISTORTION_MIX, "Low drive", 0.f, 1.f, Parameters::Config::DISTORTION_WET_DEF ));
            
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::LOW_BAND, "Low band",
                Parameters::Ranges::LOW_BAND_MIN, Parameters::Ranges::LOW_BAND_MAX, Parameters::Config::LOW_BAND_DEF
            ));
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::MID_BAND, "Mid band",
                Parameters::Ranges::MID_BAND_MIN, Parameters::Ranges::MID_BAND_MAX, Parameters::Config::MID_BAND_DEF
            ));
            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::HI_BAND, "High band",
                Parameters::Ranges::HI_BAND_MIN, Parameters::Ranges::HI_BAND_MAX, Parameters::Config::HI_BAND_DEF
            ));

            params.push_back( std::make_unique<juce::AudioParameterFloat>( Parameters::WET_DRY_MIX, "Wet / dry mix",
                0.f, 1.f, Parameters::Config::WET_DRY_MIX_DEF
            ));

            params.push_back( std::make_unique<juce::AudioParameterBool>( Parameters::REVERB_FREEZE, "Freeze", false ));    
            params.push_back( std::make_unique<juce::AudioParameterBool>( Parameters::INVERT_DIRECTION, "Invert", Parameters::Config::INVERT_DIR_DEF ));   
            
            return { params.begin(), params.end() };
        }

        /* editor */
        
        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        /* persistence */

        void getStateInformation( juce::MemoryBlock& destData ) override;
        void setStateInformation( const void* data, int sizeInBytes ) override;
        
        /* runtime state */

        bool alignWithSequencer( juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo );
        
    private:
        juce::OwnedArray<juce::IIRFilter> lowPassFilters;
        juce::OwnedArray<juce::IIRFilter> bandPassFilters;
        juce::OwnedArray<juce::IIRFilter> highPassFilters;

        // BitCrusher* bitCrusher = nullptr;
        WaveShaper* waveShaper = nullptr;
        juce::OwnedArray<DopplerEffect> lowDopplerEffects;
        juce::OwnedArray<DopplerEffect> midDopplerEffects;
        juce::OwnedArray<DopplerEffect> hiDopplerEffects;
        juce::OwnedArray<Reverb> reverbs;
        
        double _sampleRate;
        
        bool isPlaying  = false;
        int timeSigNumerator   = 4;
        int timeSigDenominator = 4;
        double tempo = 120.0;
        
        // parameters

        std::atomic<float>* lowLfoOdd;
        std::atomic<float>* lowLfoEven;
        std::atomic<float>* lowLfoLink;
        std::atomic<float>* midLfoOdd;
        std::atomic<float>* midLfoEven;
        std::atomic<float>* midLfoLink;
        std::atomic<float>* hiLfoOdd;
        std::atomic<float>* hiLfoEven;
        std::atomic<float>* hiLfoLink;
        std::atomic<float>* distortionMix;
        std::atomic<float>* lowBand;
        std::atomic<float>* midBand;
        std::atomic<float>* hiBand;
        std::atomic<float>* wetDryMix;
        std::atomic<float>* reverbFreeze;
        std::atomic<float>* invertDirection;
        
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioPluginAudioProcessor )
};
