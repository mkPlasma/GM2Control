# GM2Control

This is a VST plugin created with JUCE and the Windows MIDI API.
It is designed as a generic controller for any GM2-compatible MIDI device.

An instance of the plugin can serve as a controller for a particular device.
The GUI allows for per-channel tone selection and editing of CC parameters including filters and vibrato.

Devices are defined using a JSON file. Another JSON file is used to define the tone list.
Tones are divided into categories, and each tone has a PC, bank MSB, and bank LSB value.

Example device files can be found in the Example/ folder.
