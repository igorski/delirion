#!/bin/bash
clear
echo "Flushing build caches and output folders"
rm -rf build

echo "Creating build folders"
mkdir build
cd build

# Parse arguments

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --team_id) team_id="$2"; shift ;;
        --identity) identity="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

echo "Building project"
echo "----------------"
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

buildStatus=$?

if [ $buildStatus -eq 0 ]; then

    echo "Plugin built successfully"

    # code signing (macOS)

    echo "Signing plugins..."
    
    if [ "$identity" ]; then
        
        # Audio Unit
        AU_FILE="./delirion_artefacts/Release/AU/Delirion.component"
        codesign -s "${identity}" "${AU_FILE}" --timestamp --deep --strict --options=runtime --force
        codesign --verify --deep --verbose "${AU_FILE}"

        # VST
        VST_FILE="./delirion_artefacts/Release/VST3/Delirion.vst3"
        codesign -s "${identity}" "${VST_FILE}" --timestamp --deep --strict --options=runtime --force
        codesign --verify --deep --verbose "${VST_FILE}"
    fi
else
    echo "An error occurred during build of plugin"
fi

exit $buildStatus