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
    lfoOddAttachment  = createControl( Parameters::LFO_ODD,  lfoOddControl );
    lfoEvenAttachment = createControl( Parameters::LFO_EVEN, lfoEvenControl );
        
    setSize( 400, 300 );
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
}