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
    lowLfoOddAtt  = createControl( Parameters::LOW_LFO_ODD,  lowLfoOddControl );
    lowLfoEvenAtt = createControl( Parameters::LOW_LFO_EVEN, lowLfoEvenControl );
    lowLfoLinkAtt = createControl( Parameters::LOW_LFO_LINK, lowLfoLinkControl );
    midLfoOddAtt  = createControl( Parameters::MID_LFO_ODD,  midLfoOddControl );
    midLfoEvenAtt = createControl( Parameters::MID_LFO_EVEN, midLfoEvenControl );
    midLfoLinkAtt = createControl( Parameters::MID_LFO_LINK, midLfoLinkControl );
    hiLfoOddAtt   = createControl( Parameters::HI_LFO_ODD,   hiLfoOddControl );
    hiLfoEvenAtt  = createControl( Parameters::HI_LFO_EVEN,  hiLfoEvenControl );
    hiLfoLinkAtt  = createControl( Parameters::HI_LFO_LINK,  hiLfoLinkControl );

    bitAmountAtt = createControl( Parameters::BIT_AMOUNT, bitAmountControl );
    bitMixAtt    = createControl( Parameters::BIT_MIX,    bitMixControl );

    lowBandAtt = createControl( Parameters::LOW_BAND, lowBandControl );
    lowBandControl.setRange( Parameters::Ranges::LOW_BAND_MIN, Parameters::Ranges::LOW_BAND_MAX, 1.f );
    midBandAtt = createControl( Parameters::MID_BAND, midBandControl );
    midBandControl.setRange( Parameters::Ranges::MID_BAND_MIN, Parameters::Ranges::MID_BAND_MAX, 1.f );
    hiBandAtt  = createControl( Parameters::HI_BAND,  hiBandControl );
    hiBandControl.setRange( Parameters::Ranges::HI_BAND_MIN, Parameters::Ranges::HI_BAND_MAX, 1.f );

    reverbMixAtt    = createControl( Parameters::REVERB_MIX,    reverbMixControl );
    reverbFreezeAtt = createControl( Parameters::REVERB_FREEZE, reverbFreezeControl );

    setSize( 700, 350 );
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // nowt...
}

void AudioPluginAudioProcessorEditor::paint( juce::Graphics& g )
{
    g.fillAll( getLookAndFeel().findColour( juce::ResizableWindow::backgroundColourId ));
}

void AudioPluginAudioProcessorEditor::resized()
{
    int dialRadius  = 100;
    int dialSpacing = 75;
    int sectSpacing = 25;
    int lowSectionX = 10;
    int midSectionX = lowSectionX + ( dialSpacing * 2 ) + sectSpacing;
    int hiSectionX  = midSectionX + ( dialSpacing * 2 ) + sectSpacing;

    lowLfoOddControl.setBounds ( lowSectionX, 50, dialRadius, dialRadius );
    lowLfoEvenControl.setBounds( lowSectionX + dialSpacing, 50, dialRadius, dialRadius );
    lowLfoLinkControl.setBounds( lowSectionX + dialSpacing, 125, dialRadius, dialRadius );
    midLfoOddControl.setBounds ( midSectionX, 50, dialRadius, dialRadius );
    midLfoEvenControl.setBounds( midSectionX + dialSpacing, 50, dialRadius, dialRadius );
    midLfoLinkControl.setBounds( midSectionX + dialSpacing, 125, dialRadius, dialRadius );
    hiLfoOddControl.setBounds  ( hiSectionX, 50, dialRadius, dialRadius );
    hiLfoEvenControl.setBounds ( hiSectionX + dialSpacing, 50, dialRadius, dialRadius );
    hiLfoLinkControl.setBounds ( hiSectionX + dialSpacing, 125, dialRadius, dialRadius );
    
    bitAmountControl.setBounds( 10, 200, dialRadius, dialRadius );
    bitMixControl.setBounds   ( 10 + dialSpacing, 200, dialRadius, dialRadius );

    lowBandControl.setBounds( midSectionX, 200, dialRadius, dialRadius );
    midBandControl.setBounds( midSectionX + dialSpacing, 200, dialRadius, dialRadius );
    hiBandControl.setBounds ( midSectionX + dialSpacing * 2, 200, dialRadius, dialRadius );
    
    int reverbX = 200 + dialSpacing * 2 + sectSpacing;

    reverbMixControl.setBounds   ( reverbX, 200, dialRadius, dialRadius );
    reverbFreezeControl.setBounds( reverbX + dialSpacing, 200, dialRadius, dialRadius );
}