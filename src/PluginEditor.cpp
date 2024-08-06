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
#include "BinaryData.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor( AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& state )
    : AudioProcessorEditor( &p ), parameters( state )
{
    lowLfoOddAtt  = createControl( Parameters::LOW_LFO_ODD,  lowLfoOddControl,  true );
    lowLfoEvenAtt = createControl( Parameters::LOW_LFO_EVEN, lowLfoEvenControl, true );
    lowLfoLinkAtt = createControl( Parameters::LOW_LFO_LINK, lowLfoLinkControl );
    midLfoOddAtt  = createControl( Parameters::MID_LFO_ODD,  midLfoOddControl,  true );
    midLfoEvenAtt = createControl( Parameters::MID_LFO_EVEN, midLfoEvenControl, true );
    midLfoLinkAtt = createControl( Parameters::MID_LFO_LINK, midLfoLinkControl );
    hiLfoOddAtt   = createControl( Parameters::HI_LFO_ODD,   hiLfoOddControl,   true );
    hiLfoEvenAtt  = createControl( Parameters::HI_LFO_EVEN,  hiLfoEvenControl,  true );
    hiLfoLinkAtt  = createControl( Parameters::HI_LFO_LINK,  hiLfoLinkControl );

    distMixAtt = createControl( Parameters::DISTORTION_MIX, distMixControl, false );

    lowBandAtt = createControl( Parameters::LOW_BAND, lowBandControl, true );
    lowBandControl.setRange( Parameters::Ranges::LOW_BAND_MIN, Parameters::Ranges::LOW_BAND_MAX, 1.f );
    midBandAtt = createControl( Parameters::MID_BAND, midBandControl, true );
    midBandControl.setRange( Parameters::Ranges::MID_BAND_MIN, Parameters::Ranges::MID_BAND_MAX, 1.f );
    hiBandAtt  = createControl( Parameters::HI_BAND,  hiBandControl, true );
    hiBandControl.setRange( Parameters::Ranges::HI_BAND_MIN, Parameters::Ranges::HI_BAND_MAX, 1.f );

    dryWetMixAtt = createControl( Parameters::WET_DRY_MIX, dryWetMixControl, false );
    
    reverbFreezeAtt = createControl( Parameters::REVERB_FREEZE, reverbFreezeControl );

    invertDirectionAtt = createControl( Parameters::INVERT_DIRECTION, invertDirectionControl );

    scaledWidth  = static_cast<int>( ceil( WIDTH / 2 ));
    scaledHeight = static_cast<int>( ceil( HEIGHT / 2 ));

    setSize( scaledWidth, scaledHeight );
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // nowt...
}

void AudioPluginAudioProcessorEditor::paint( juce::Graphics& g )
{
    // g.fillAll( getLookAndFeel().findColour( juce::ResizableWindow::backgroundColourId ));

    juce::Image background = juce::ImageCache::getFromMemory( BinaryData::background_png, BinaryData::background_pngSize );
    g.drawImage( background, 0, 0, scaledWidth, scaledHeight, 0, 0, WIDTH, HEIGHT, false );

    int scaledVersionWidth  = static_cast<int>( ceil( VERSION_WIDTH  / 2 ));
    int scaledVersionHeight = static_cast<int>( ceil( VERSION_HEIGHT / 2 ));

    juce::Image version = juce::ImageCache::getFromMemory( BinaryData::version_png, BinaryData::version_pngSize );
    g.drawImage(
        version,
        scaledWidth - ( scaledVersionWidth + 15 ), scaledHeight - 37, scaledVersionWidth, scaledVersionHeight,
        0, 0, VERSION_WIDTH, VERSION_HEIGHT, false
    );
}

void AudioPluginAudioProcessorEditor::resized()
{
    int dialRadius  = 80;
    int dialSpacing = 81;
    int lowSectionX = 65;
    int lowSectionY = 110;
    int midSectionX = lowSectionX + 207;
    int midSectionY = lowSectionY + 105;
    int hiSectionY  = lowSectionY;
    int hiSectionX  = midSectionX + 206;

    lowLfoOddControl.setBounds ( lowSectionX, lowSectionY, dialRadius, dialRadius );
    lowLfoEvenControl.setBounds( lowSectionX + dialSpacing, lowSectionY, dialRadius, dialRadius );
    lowLfoLinkControl.setBounds( lowSectionX + 68, lowSectionY + 45, dialRadius, dialRadius );
    lowBandControl.setBounds   ( lowSectionX + 42, 213, dialRadius, dialRadius );
  
    midLfoOddControl.setBounds ( midSectionX, midSectionY, dialRadius, dialRadius );
    midLfoEvenControl.setBounds( midSectionX + dialSpacing, midSectionY, dialRadius, dialRadius );
    midLfoLinkControl.setBounds( midSectionX + 68, lowSectionY + 45, dialRadius, dialRadius );
    midBandControl.setBounds   ( midSectionX + 42, 103, dialRadius, dialRadius );
   
    hiLfoOddControl.setBounds ( hiSectionX, hiSectionY, dialRadius, dialRadius );
    hiLfoEvenControl.setBounds( hiSectionX + dialSpacing, hiSectionY, dialRadius, dialRadius );
    hiLfoLinkControl.setBounds( hiSectionX + 68, hiSectionY + 45, dialRadius, dialRadius );
    hiBandControl.setBounds   ( hiSectionX + 42, 213, dialRadius, dialRadius );
    
    distMixControl.setBounds( 108, 323, dialRadius, dialRadius );

    dryWetMixControl.setBounds( 519, 323, dialRadius, dialRadius );
    
    reverbFreezeControl.setBounds( 365, 343, dialRadius, dialRadius );

    invertDirectionControl.setBounds( 365, 49, dialRadius, dialRadius );
}