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
#ifndef __PARAM_LISTENER_H_INCLUDED__
#define __PARAM_LISTENER_H_INCLUDED__

#include <juce_audio_processors/juce_audio_processors.h>
#include "ParameterSubscriber.h"

class ParameterListener : public juce::ValueTree::Listener
{
    public:
        ParameterListener( ParameterSubscriber& subscriber, juce::AudioProcessorValueTreeState& vts ) : subscriberRef( subscriber ), valueTreeState( vts )
        {
            valueTreeState.state.addListener( this );
        }

        ~ParameterListener() override
        {
            valueTreeState.state.removeListener( this );
        }

        void valueTreePropertyChanged( juce::ValueTree& tree, const juce::Identifier& property ) override
        {
            juce::ignoreUnused( tree, property );
            subscriberRef.updateParameters();
        }

        /*
        void valueTreeChildAdded       ( juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded ) override {}
        void valueTreeChildRemoved     ( juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int ) override {}
        void valueTreeChildOrderChanged( juce::ValueTree& parentTree, int, int ) override {}
        void valueTreeParentChanged    ( juce::ValueTree& treeWhoseParentHasChanged ) override {}
        */

    private:
        ParameterSubscriber& subscriberRef;
        juce::AudioProcessorValueTreeState& valueTreeState;
};

#endif
