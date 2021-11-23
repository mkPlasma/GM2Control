#include"MidiOut.h"

#include"Controller.h"
#include"HexFunc.h"
#include<string>
#include<regex>
#include<vector>
#include<assert.h>

using std::regex;
using std::smatch;
using std::regex_search;
using std::vector;


MidiOut::MidiOut(Controller& controller) : _controller(controller), _open(false), _bufferIndex(0) {
}

MidiOut::~MidiOut(){
	closeDevice();
}

void MidiOut::openDevice(){

	// No device loaded, return
	if(!_controller.getDevice().loaded)
		return;


	// Try opening MIDI device
	auto devices = MidiOutput::getAvailableDevices();

	for(auto device : devices){
		if(device.name.toStdString() == _controller.getDevice().midiDeviceName){
			_out = MidiOutput::openDevice(device.identifier);
			break;
		}
	}

	_open = _out.get() != nullptr;
}

void MidiOut::closeDevice(){
	if(_open){
		_out.reset();
		_open = false;
	}
}

void MidiOut::shortMsg(MidiMessage msg){

	if(!_open)
		return;

	_buffer.addEvent(msg, _bufferIndex++);
}

void MidiOut::shortMsg(uint8* msg){

	if(!_open)
		return;

	_buffer.addEvent(msg, 3, _bufferIndex++);
}

void MidiOut::cc(int channel, int cc, int val){
	
	uint8 msg[] = {
		(uint8)(0xb0 + channel),
		(uint8)cc,
		(uint8)val
	};

	shortMsg(msg);
}

void MidiOut::setTone(int channel, int pc, int msb, int lsb){

	if(!_open)
		return;

	// Bank select
	cc(channel, 0, msb);
	cc(channel, 0x20, lsb);

	// Program change
	uint8 msg[] = {
		(uint8)(0xc0 + channel),
		(uint8)pc,
		(uint8)0
	};

	shortMsg(msg);
}

void MidiOut::sysex(string data){

	if(!_open)
		return;

	// Unused as of now
	/*
	// MIDI data
	vector<char> msg;

	// 'Header' bytes
	msg.push_back((BYTE)0xf0);
	msg.push_back((BYTE)_controller.getDevice().sysexHeaderByte1);
	msg.push_back((BYTE)_controller.getDevice().sysexHeaderByte2);

	// Get data
	regex rgx("([\\dA-Fa-f]{2})\\s*");
	smatch matches;

	// TODO: Extract data from string, convert to bytes, and add to MIDI data vector
	if(regex_search(data, matches, rgx)){

	}

	// EOX
	msg.push_back((BYTE)0xf7);


	MIDIHDR hdr;
	hdr.lpData = msg.data();
	hdr.dwBufferLength = (DWORD)msg.size();
	hdr.dwBytesRecorded = (DWORD)msg.size();
	hdr.dwFlags = 0;

	// Send message
	midiOutPrepareHeader(_device, &hdr, sizeof(hdr));
	auto error = midiOutLongMsg(_device, &hdr, sizeof(hdr));

	// Check MIDI error
	//assert(error == 0);

	// Wait to unprepare header
	if(error == 0)
		while((hdr.dwFlags & MHDR_DONE) != MHDR_DONE);

	midiOutUnprepareHeader(_device, &hdr, sizeof(hdr));
	*/
}

void MidiOut::sendMessagesInBuffer(){

	// Send messages
	_out.get()->sendBlockOfMessagesNow(_buffer);
	
	// Clear buffer
	_buffer.clear();
	_bufferIndex = 0;
}

bool MidiOut::isOpen(){
	return _open;
}
