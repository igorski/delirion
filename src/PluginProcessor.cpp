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
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DopplerEffect.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor(): AudioProcessor( BusesProperties()
    #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
        .withInput( "Input",  juce::AudioChannelSet::stereo(), true )
        #endif
        .withOutput( "Output", juce::AudioChannelSet::stereo(), true )
    #endif
    ),
    ParameterSubscriber(),
    parameters( *this, nullptr, "PARAMETERS", createParameterLayout()),
    parameterListener( *this, parameters ) 
{
    // grab a reference to all automatable parameters

    lfoOdd  = parameters.getRawParameterValue( Parameters::LFO_ODD );
    lfoEven = parameters.getRawParameterValue( Parameters::LFO_EVEN );
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    // nowt...
}

/* configuration */

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported( const BusesLayout& layouts ) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused( layouts );
    return true;
  #else
    if ( layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
         layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) {
        return false;
    }

   #if ! JucePlugin_IsSynth
    if ( layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) {
        return false;
    }
   #endif

    return true;
  #endif
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

/* programs */

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram( int index )
{
    juce::ignoreUnused( index );
}

const juce::String AudioPluginAudioProcessor::getProgramName( int index )
{
    juce::ignoreUnused( index );
    return {};
}

void AudioPluginAudioProcessor::changeProgramName( int index, const juce::String& newName )
{
    juce::ignoreUnused( index, newName );
}

/* automatable parameters */

void AudioPluginAudioProcessor::updateParameters()
{
    int channelAmount = getTotalNumOutputChannels();

    for ( int channel = 0; channel < channelAmount; ++channel ) {
        dopplerEffects[ channel ]->setSpeed( channel % 2 == 0 ? *lfoOdd : *lfoEven );
    }
}

/* resource management */

void AudioPluginAudioProcessor::prepareToPlay( double sampleRate, int samplesPerBlock )
{
    juce::ignoreUnused( sampleRate, samplesPerBlock );

    // dispose previously allocated resources
    releaseResources();

    int channelAmount = getTotalNumOutputChannels();

    for ( int i = 0; i < channelAmount; ++i )
    {
        lowPassFilters.add ( new juce::IIRFilter());
        bandPassFilters.add( new juce::IIRFilter());
        highPassFilters.add( new juce::IIRFilter());

        lowPassFilters [ i ]->setCoefficients( juce::IIRCoefficients::makeLowPass ( sampleRate, 200.0 ));
        bandPassFilters[ i ]->setCoefficients( juce::IIRCoefficients::makeBandPass( sampleRate, 1000.0, 1.0 ));
        highPassFilters[ i ]->setCoefficients( juce::IIRCoefficients::makeHighPass( sampleRate, 5000.0 ));

        dopplerEffects.add( new DopplerEffect(( float ) sampleRate, samplesPerBlock ));
    }
    updateParameters();
}

void AudioPluginAudioProcessor::releaseResources()
{
    lowPassFilters.clear();
    bandPassFilters.clear();
    highPassFilters.clear();

    dopplerEffects.clear();
}

/* rendering */

void AudioPluginAudioProcessor::processBlock( juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages )
{
    juce::ignoreUnused( midiMessages );

    juce::ScopedNoDenormals noDenormals;

    int channelAmount = buffer.getNumChannels();
    int bufferSize    = buffer.getNumSamples();

    // Create temporary buffers for each band
    juce::AudioBuffer<float> lowBuffer ( channelAmount, bufferSize );
    juce::AudioBuffer<float> midBuffer ( channelAmount, bufferSize );
    juce::AudioBuffer<float> highBuffer( channelAmount, bufferSize );

    for ( int channel = 0; channel < channelAmount; ++channel )
    {
        if ( buffer.getReadPointer( channel ) == nullptr ) {
            continue;
        }

       // lowBuffer.copyFrom ( channel, 0, buffer, channel, 0, bufferSize );
        midBuffer.copyFrom ( channel, 0, buffer, channel, 0, bufferSize );
       // highBuffer.copyFrom( channel, 0, buffer, channel, 0, bufferSize );

        // apply the Doppler effect

        dopplerEffects[ channel ]->apply( midBuffer, channel );

        // apply the filtering

     //   lowPassFilters [ channel ]->processSamples( lowBuffer.getWritePointer ( channel ), bufferSize );
        bandPassFilters[ channel ]->processSamples( midBuffer.getWritePointer ( channel ), bufferSize );
      //  highPassFilters[ channel ]->processSamples( highBuffer.getWritePointer( channel ), bufferSize );
    
        // auto* channelData = buffer.getWritePointer( channel );

        for ( int i = 0; i < bufferSize; ++i ) {
            buffer.setSample(channel, i,
                             //  lowBuffer.getSample(channel, i) +
                               midBuffer.getSample(channel, i) //+
                               //  highBuffer.getSample( channel, i )
                                );
        }
    }
}

/* editor */

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor( *this, parameters );
}

/* persistence */

void AudioPluginAudioProcessor::getStateInformation( juce::MemoryBlock& destData )
{
    juce::MemoryOutputStream stream( destData, true );
    parameters.state.writeToStream( stream );
}

void AudioPluginAudioProcessor::setStateInformation( const void* data, int sizeInBytes )
{
    juce::ValueTree tree = juce::ValueTree::readFromData( data, static_cast<unsigned long>( sizeInBytes ));
    if ( tree.isValid()) {
        parameters.state = tree;
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}