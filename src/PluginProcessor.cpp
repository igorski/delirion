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
    // grab a reference to all automatable parameters and initialize the values (to their defined defaults)

    lowLfoOdd  = parameters.getRawParameterValue( Parameters::LOW_LFO_ODD );
    lowLfoEven = parameters.getRawParameterValue( Parameters::LOW_LFO_EVEN );
    lowLfoLink = parameters.getRawParameterValue( Parameters::LOW_LFO_LINK );
    midLfoOdd  = parameters.getRawParameterValue( Parameters::MID_LFO_ODD );
    midLfoEven = parameters.getRawParameterValue( Parameters::MID_LFO_EVEN );
    midLfoLink = parameters.getRawParameterValue( Parameters::MID_LFO_LINK );
    hiLfoOdd   = parameters.getRawParameterValue( Parameters::HI_LFO_ODD );
    hiLfoEven  = parameters.getRawParameterValue( Parameters::HI_LFO_EVEN );
    hiLfoLink  = parameters.getRawParameterValue( Parameters::HI_LFO_LINK );

    bitAmount = parameters.getRawParameterValue( Parameters::BIT_AMOUNT );
    bitMix    = parameters.getRawParameterValue( Parameters::BIT_MIX );

    lowBand = parameters.getRawParameterValue( Parameters::LOW_BAND );
    midBand = parameters.getRawParameterValue( Parameters::MID_BAND );
    hiBand  = parameters.getRawParameterValue( Parameters::HI_BAND );

    wetDryMix = parameters.getRawParameterValue( Parameters::WET_DRY_MIX );

    reverbFreeze = parameters.getRawParameterValue( Parameters::REVERB_FREEZE );

    invertDirection = parameters.getRawParameterValue( Parameters::INVERT_DIRECTION );
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
    bitCrusher->setAmount( *bitAmount );
    bitCrusher->setOutputMix( *bitMix );

    int channelAmount = getTotalNumOutputChannels();

    bool linkLow = *lowLfoLink >= 0.5f;
    bool linkMid = *midLfoLink >= 0.5f;
    bool linkHi  = *hiLfoLink  >= 0.5f;
    bool freeze  = *reverbFreeze >= 0.5f;
    bool invert  = *invertDirection >= 0.5f;

    for ( int channel = 0; channel < channelAmount; ++channel ) {
        bool isOddChannel = channel % 2 == 0;

        lowDopplerEffects[ channel ]->setProperties( linkLow || isOddChannel ? *lowLfoOdd : *lowLfoEven, invert );
        midDopplerEffects[ channel ]->setProperties( linkMid || isOddChannel ? *midLfoOdd : *midLfoEven, invert );
        hiDopplerEffects [ channel ]->setProperties( linkHi  || isOddChannel ? *hiLfoOdd  : *hiLfoEven,  invert );

        reverbs[ channel ]->setWet( freeze ? 1.f : 0.f );
        reverbs[ channel ]->setDry( freeze ? 0.f : 1.f  );
        reverbs[ channel ]->setMode( freeze ? 1 : 0 );

        // TODO check whether this is expensive and cache the last created coefficients

        lowPassFilters [ channel ]->setCoefficients( juce::IIRCoefficients::makeLowPass ( _sampleRate, *lowBand ));
        bandPassFilters[ channel ]->setCoefficients( juce::IIRCoefficients::makeBandPass( _sampleRate, *midBand, 1.0 ));
        highPassFilters[ channel ]->setCoefficients( juce::IIRCoefficients::makeHighPass( _sampleRate, *hiBand ));
    }
}

/* resource management */

void AudioPluginAudioProcessor::prepareToPlay( double sampleRate, int samplesPerBlock )
{
    _sampleRate = sampleRate;

    // dispose previously allocated resources
    releaseResources();

    int channelAmount = getTotalNumOutputChannels();

    for ( int i = 0; i < channelAmount; ++i )
    {
        lowPassFilters.add ( new juce::IIRFilter());
        bandPassFilters.add( new juce::IIRFilter());
        highPassFilters.add( new juce::IIRFilter());

        lowPassFilters [ i ]->setCoefficients( juce::IIRCoefficients::makeLowPass ( sampleRate, Parameters::Config::LOW_BAND_DEF ));
        bandPassFilters[ i ]->setCoefficients( juce::IIRCoefficients::makeBandPass( sampleRate, Parameters::Config::MID_BAND_DEF, 1.0 ));
        highPassFilters[ i ]->setCoefficients( juce::IIRCoefficients::makeHighPass( sampleRate, Parameters::Config::HI_BAND_DEF ));

        lowDopplerEffects.add( new DopplerEffect( sampleRate, samplesPerBlock ));
        midDopplerEffects.add( new DopplerEffect( sampleRate, samplesPerBlock ));
        hiDopplerEffects.add ( new DopplerEffect( sampleRate, samplesPerBlock ));

        reverbs.add( new Reverb( sampleRate, Parameters::Config::REVERB_WIDTH_DEF, Parameters::Config::REVERB_SIZE_DEF ));
    }
    bitCrusher = new BitCrusher( Parameters::Config::BITCRUSHER_AMT_DEF, 1.f, Parameters::Config::BITCRUSHER_WET_DEF );
    
    // align values with model
    updateParameters();
}

void AudioPluginAudioProcessor::releaseResources()
{
    lowPassFilters.clear();
    bandPassFilters.clear();
    highPassFilters.clear();

    lowDopplerEffects.clear();
    midDopplerEffects.clear();
    hiDopplerEffects.clear();

    reverbs.clear();

    if ( bitCrusher != nullptr ) {
        delete bitCrusher;
        bitCrusher = nullptr;
    }
}

/* rendering */

void AudioPluginAudioProcessor::processBlock( juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages )
{
    juce::ignoreUnused( midiMessages );
    juce::ScopedNoDenormals noDenormals;

    auto currentPosition = getPlayHead()->getPosition();

    if ( currentPosition.hasValue()) {
        bool wasPlaying = isPlaying;
        isPlaying = currentPosition->getIsPlaying();

        if ( !wasPlaying && isPlaying ) {
            resetOscillators();
        }
    }
  
    int channelAmount = buffer.getNumChannels();
    int bufferSize    = buffer.getNumSamples();

    float dryMix = 1.f - *wetDryMix;
    float wetMix = *wetDryMix;

    // Create temporary buffers for each band
    juce::AudioBuffer<float> lowBuffer( channelAmount, bufferSize );
    juce::AudioBuffer<float> midBuffer( channelAmount, bufferSize );
    juce::AudioBuffer<float> hiBuffer ( channelAmount, bufferSize );

    for ( int channel = 0; channel < channelAmount; ++channel )
    {
        if ( buffer.getReadPointer( channel ) == nullptr ) {
            continue;
        }

        lowBuffer.copyFrom ( channel, 0, buffer, channel, 0, bufferSize );
        midBuffer.copyFrom ( channel, 0, buffer, channel, 0, bufferSize );
        hiBuffer.copyFrom( channel, 0, buffer, channel, 0, bufferSize );

        lowDopplerEffects[ channel ]->apply( lowBuffer, channel );
        midDopplerEffects[ channel ]->apply( midBuffer, channel );
        hiDopplerEffects [ channel ]->apply( hiBuffer,  channel );

        // apply the effects

        bitCrusher->apply( lowBuffer, channel );

        reverbs[ channel ]->apply( midBuffer, channel );
        
        // apply the filtering

        lowPassFilters [ channel ]->processSamples( lowBuffer.getWritePointer( channel ), bufferSize );
        bandPassFilters[ channel ]->processSamples( midBuffer.getWritePointer( channel ), bufferSize );
        highPassFilters[ channel ]->processSamples( hiBuffer.getWritePointer ( channel ), bufferSize );

        // write the effected buffer into the output
    
        for ( int i = 0; i < bufferSize; ++i ) {
            auto input = buffer.getSample( channel, i ) * dryMix;

            buffer.setSample(
                channel, i,
                input + (
                    lowBuffer.getSample( channel, i ) +
                    midBuffer.getSample( channel, i ) +
                    hiBuffer.getSample ( channel, i )
                ) * wetMix
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

/* runtime state */

void AudioPluginAudioProcessor::resetOscillators()
{
    int channelAmount = getTotalNumOutputChannels();

    for ( int channel = 0; channel < channelAmount; ++channel ) {
        lowDopplerEffects[ channel ]->resetOscillators();
        midDopplerEffects[ channel ]->resetOscillators();
        hiDopplerEffects [ channel ]->resetOscillators();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}