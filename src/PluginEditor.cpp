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
    int lowSectionX = 71;
    int lowSectionY = 112;
    int midSectionX = lowSectionX + 207;
    int hiSectionY  = lowSectionY;
    int hiSectionX  = midSectionX + 205;

    lowLfoOddControl.setBounds ( lowSectionX, lowSectionY, ROTARY_SIZE, ROTARY_SIZE );
    lowLfoEvenControl.setBounds( lowSectionX + ROTARY_MARGIN, lowSectionY, ROTARY_SIZE, ROTARY_SIZE );
    lowLfoLinkControl.setBounds( lowSectionX + 62, lowSectionY + 44, SLIDER_WIDTH, SLIDER_WIDTH );
    lowBandControl.setBounds   ( lowSectionX + 41, 218, ROTARY_SIZE, ROTARY_SIZE );
  
    midLfoOddControl.setBounds ( midSectionX, lowBandControl.getY() + 4, ROTARY_SIZE, ROTARY_SIZE );
    midLfoEvenControl.setBounds( midSectionX + ROTARY_MARGIN, midLfoOddControl.getY(), ROTARY_SIZE, ROTARY_SIZE );
    midLfoLinkControl.setBounds( midSectionX + 61, lowLfoLinkControl.getY(), SLIDER_WIDTH, SLIDER_WIDTH );
    midBandControl.setBounds   ( midSectionX + 40, lowSectionY, ROTARY_SIZE, ROTARY_SIZE );
   
    hiLfoOddControl.setBounds ( hiSectionX, hiSectionY, ROTARY_SIZE, ROTARY_SIZE );
    hiLfoEvenControl.setBounds( hiSectionX + ROTARY_MARGIN, hiSectionY, ROTARY_SIZE, ROTARY_SIZE );
    hiLfoLinkControl.setBounds( hiSectionX + 63, lowLfoLinkControl.getY(), SLIDER_WIDTH, SLIDER_WIDTH );
    hiBandControl.setBounds   ( hiSectionX + 40, lowBandControl.getY(), ROTARY_SIZE, ROTARY_SIZE );
    
    distMixControl.setBounds( 108, 323, SLIDER_WIDTH, SLIDER_WIDTH );

    dryWetMixControl.setBounds( 519, 323, SLIDER_WIDTH, SLIDER_WIDTH );
    
    reverbFreezeControl.setBounds( 365, 343, SLIDER_WIDTH, SLIDER_WIDTH );

    invertDirectionControl.setBounds( 365, 49, SLIDER_WIDTH, SLIDER_WIDTH );
}