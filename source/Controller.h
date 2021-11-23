#pragma once

#include<JuceHeader.h>
#include"ToneList.h"
#include"MidiOut.h"
#include<string>

using juce::AudioProcessorValueTreeState;
using std::string;


struct Device{
	bool loaded;

	string path;
	string name;
	string midiDeviceName;
	string toneListPath;

	char sysexHeaderByte1;
	char sysexHeaderByte2;

	ToneList toneList;

	Device() : loaded(false) {}
};


class Controller{

	Device _device;
	MidiOut _midiOut;
	AudioProcessorValueTreeState& _parameters;

	// Selected channel
	int _channel;

	// Track note on/off for animation
	bool _noteOn[16][128];

public:
	Controller(AudioProcessorValueTreeState& parameters);

	void loadDevice(const string& path);

	void setNote(int channel, int note, bool on);
	bool allNotesInChannelOff(int channel);

	void setParameter(const string& parameter, int value);
	void setParameter(const string& parameter, float value);
	float getParameter(const string& parameter);

	const Device& getDevice();
	MidiOut& getMidiOut();
	AudioProcessorValueTreeState& getParameters();

	void setChannel(int channel);
	int getChannel();
};
