# Doppler

Doppler was built using the [JUCE framework](https://github.com/juce-framework/JUCE).

## The [Issue Tracker](https://github.com/igorski/doppler/issues) is your point of contact

Bug reports, feature requests, questions and discussions are welcome on the GitHub Issue Tracker, please do not send e-mails through the development website. However, please search before posting to avoid duplicates, and limit to one issue per post.

Please vote on feature requests by using the Thumbs Up/Down reaction on the first post.

## Setup

You will need to have CMake and a suitable C compiler installed. JUCE* is a submodule of this repository so
all library dependencies are handled when cloning this repository recursively. When cloning withour recursion,
run the following to clone the JUCE framework into a subfolder `JUCE`, relative to this repository root.

```
git clone https://github.com/juce-framework/JUCE
```

_*The version used to create this plugin was [7.0.12](https://github.com/juce-framework/JUCE/releases/tag/7.0.12)_

### Build

You can create a runtime by running the below:

```
cmake . -B build -G <GENERATOR_RUNTIME>
```

where the G flag is optional (defaults to the common generator for your platform) and provided `<GENERATOR_RUNTIME>` should reflect a custom generator to use (e.g. `XCode` (on Mac), `Unix makefiles`, `Ninja`, etc.)