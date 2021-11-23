#pragma once

#include<JuceHeader.h>
#include<string>
#include"ToneList.h"
#include"MidiOut.h"

using juce::AudioProcessorValueTreeState;
using std::string;

class GM2ControlProcessor;


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

	GM2ControlProcessor& _processor;
	AudioProcessorValueTreeState& _parameters;

	Device _device;
	MidiOut _midiOut;

	// Selected channel
	int _channel;

	// Track note on/off for animation
	bool _noteOn[16][128];

public:
	Controller(GM2ControlProcessor& processor);

	void loadDevice(const string& path);

	void setStatus(string status);

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
