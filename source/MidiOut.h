#pragma once

#include<windows.h>
#include<mmsystem.h>
#include<string>

using std::string;

class Controller;


class MidiOut{

	Controller& _controller;

	int _midiDevNum;
	bool _open;
	HMIDIOUT _device;

public:
	MidiOut(Controller& controller);
	~MidiOut();

	void openDevice();
	void closeDevice();

	void shortMsg(DWORD data);
	void cc(int channel, int cc, int val);
	void setTone(int channel, int pc, int msb, int lsb);
	void sysex(string data);

	bool isOpen();
};