#
# Copyright (c) 2024 Igor Zinken https://www.igorski.nl
# Built using the JUCE framework https://github.com/juce-framework/JUCE
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(PROJECT_NAME "delirion") # equals build target
set(PROJECT_VERSION 1.0.1)
set(PLUGIN_NAME "Delirion")

######################
# JUCE configuration #
######################

# find_package(JUCE CONFIG REQUIRED)      # When JUCE is installed globally on the system
add_subdirectory(JUCE)                    # When JUCE is a Git submodule in the ./JUCE folder (default)

if (APPLE)
    set(PLUGIN_FORMATS VST3 AU)
else()
    set(PLUGIN_FORMATS VST3)
endif()

########################################
# Platform detection and configuration #
########################################

if(UNIX)
    if(XCODE)
       # Xcode specific flags can go here, use `elseif(APPLE)` on the next line in case
       # it is necessary to distinguish between different generators on macOS
       # set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "" FORCE)
       # set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXXXXXXXXX" CACHE STRING "" FORCE)
    endif()
    if(APPLE)
       # TODO make this work to support High Sierra and up
       # set(CMAKE_OSX_SYSROOT macosx10.13)
       # set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    else()
        set(LINUX true)
    endif()
else()
    ## spotted to not be set by default on VS CLI. Here we assume any non-Unix
    ## OS MUST be Windows. Then again that's the only other intended target for this project...
    set(WIN true)
endif()

##################
# macOS specific #
##################

if (APPLE)
    option(UniversalBinary "Build universal binary for Mac" ON) # support both Intel and Apple Mx processors
    if (UniversalBinary)
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
    endif ()
endif()

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin(${PROJECT_NAME}
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME igorski.nl                     # Specify the name of the plugin's author
    IS_SYNTH FALSE                              # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE                # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE IGOR               # A four-character manufacturer id with at least one upper-case character, used for plugin UID generation
    PLUGIN_CODE D0pp                            # A unique four-character plugin id with exactly one upper-case character, used for plugin UID generation
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS ${PLUGIN_FORMATS}                   # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "${PLUGIN_NAME}"               # The name of the final executable, which can differ from the target name
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0              # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0                 # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_DISPLAY_SPLASH_SCREEN=0    # only when source remains available under GPL3 license!
        JUCE_REPORT_APP_USAGE=0         # disabled by default in JUCE 6
        JUCE_VST3_CAN_REPLACE_VST2=0
    )

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

target_sources(${PROJECT_NAME}
    PRIVATE
        # src/modules/bitcrusher/Bitcrusher.cpp
        src/modules/doppler/DopplerEffect.cpp
        src/modules/oscillator/LFO.cpp
        src/modules/reverb/Allpass.cpp
        src/modules/reverb/Comb.cpp
        src/modules/reverb/Reverb.cpp
        src/modules/waveshaper/WaveShaper.cpp
        src/PluginEditor.cpp
        src/PluginProcessor.cpp
    )

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

juce_add_binary_data(PluginResources SOURCES
    design/resources/background.png
    design/resources/version.png
)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        PluginResources
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
