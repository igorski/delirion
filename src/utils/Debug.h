#ifndef __DEBUG_H_INCLUDED__
#define __DEBUG_H_INCLUDED__

#include <juce_audio_processors/juce_audio_processors.h>

class Debug
{
    public:
        Debug()
        {
            logFile = juce::File::getSpecialLocation( juce::File::userDocumentsDirectory ).getChildFile( "plugin_log.txt" );

            // Create or overwrite the log file
            if ( logFile.existsAsFile()) {
                logFile.deleteFile();
            }
            logFile.create();
        }

        void log( const juce::String& message )
        {
            juce::FileOutputStream outputStream( logFile );

            if ( outputStream.openedOk()) {
                juce::String timeStampedMessage = juce::Time::getCurrentTime().toString( true, true ) + ": " + message + "\n";
                outputStream.writeText( timeStampedMessage, false, false, nullptr );
            }
        }

    private:
        juce::File logFile;
};

#endif