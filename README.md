# Doppler

Doppler was built using the [JUCE framework](https://github.com/juce-framework/JUCE).

## Setup

You will need to have CMake and a suitable C compiler installed. JUCE* is a submodule of this repository so
all library dependencies are handled upon cloning this repository.

_*The version used to create this plugin was [7.0.12](https://github.com/juce-framework/JUCE/releases/tag/7.0.12)_

You can create a runtime by running the below:

```
cmake . -B build -G <GENERATOR_RUNTIME>
```

where the G flag is optional (default to common generator for your platform) and provided `<GENERATOR_RUNTIME>` should reflect a custom generator to use (e.g. XCode on Mac, "Unix makefiles", Ninja, etc.)