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
#include "Parameters.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor( AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& state )
    : AudioProcessorEditor( &p ), processorRef( p ), parameters( state )
{
    createControl( Parameters::LFO_ODD,  lfoOddControl );
    createControl( Parameters::LFO_EVEN, lfoEvenControl );

    createControl( Parameters::BIT_AMOUNT, bitAmountControl );
    createControl( Parameters::BIT_MIX,    bitMixControl );

    createControl( Parameters::LOW_BAND, lowBandControl );
    lowBandControl.setRange( Parameters::Ranges::LOW_BAND_MIN, Parameters::Ranges::LOW_BAND_MAX, 1.f );
    createControl( Parameters::MID_BAND, midBandControl );
    midBandControl.setRange( Parameters::Ranges::MID_BAND_MIN, Parameters::Ranges::MID_BAND_MAX, 1.f );
    createControl( Parameters::HI_BAND,  hiBandControl );
    hiBandControl.setRange( Parameters::Ranges::HI_BAND_MIN, Parameters::Ranges::HI_BAND_MAX, 1.f );

    createControl( Parameters::REVERB_MIX,    reverbMixControl );
    createControl( Parameters::REVERB_FREEZE, reverbFreezeControl );

    setSize( 400, 400 );
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // nowt...
}

void AudioPluginAudioProcessorEditor::paint( juce::Graphics& g )
{
    g.fillAll( getLookAndFeel().findColour( juce::ResizableWindow::backgroundColourId ));

    g.setColour( juce::Colours::white );
    g.setFont( 15.0f );
    g.drawFittedText( "DOPPLER", getLocalBounds(), juce::Justification::centred, 1 );
}

void AudioPluginAudioProcessorEditor::resized()
{
    lfoOddControl.setBounds ( 50, 50, 100, 100 );
    lfoEvenControl.setBounds( 150, 50, 100, 100 );
    
    bitAmountControl.setBounds( 50, 100, 100, 100 );
    bitMixControl.setBounds( 150, 100, 100, 100 );

    lowBandControl.setBounds( 50, 0, 100, 100 );
    midBandControl.setBounds( 150, 0, 100, 100 );
    hiBandControl.setBounds( 250, 0, 100, 100 );

    reverbMixControl.setBounds( 50, 200, 100, 100 );
    reverbFreezeControl.setBounds( 150, 200, 100, 100 );
}