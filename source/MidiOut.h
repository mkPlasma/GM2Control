#pragma once

#include<JuceHeader.h>
#include<string>

using namespace juce;
using std::string;


class Controller;


class MidiOut{

	Controller& _controller;

	std::unique_ptr<MidiOutput> _out;
	bool _open;

	MidiBuffer _buffer;
	int _bufferIndex;

public:
	MidiOut(Controller& controller);
	~MidiOut();

	void openDevice();
	void closeDevice();

	void shortMsg(MidiMessage msg);
	void shortMsg(uint8* msg);

	void cc(int channel, int cc, int val);
	void setTone(int channel, int pc, int msb, int lsb);
	void sysex(string data);

	void sendMessagesInBuffer();

	bool isOpen();
};