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


MidiOut::MidiOut(Controller& controller) : _controller(controller), _midiDevNum(-1), _open(false), _device(NULL) {
	
}

MidiOut::~MidiOut(){
	closeDevice();
}

void MidiOut::openDevice(){

	// No device loaded, return
	if(!_controller.getDevice().loaded)
		return;


	MIDIOUTCAPS caps;

	// Find MIDI device
	for(int i = 0; i < (int)midiOutGetNumDevs(); i++){
		midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));

		if((std::string)caps.szPname == _controller.getDevice().midiDeviceName){
			_midiDevNum = i;
			break;
		}
	}

	// MIDI device not found
	if(_midiDevNum == -1)
		return;

	// Open device
	MMRESULT rs = midiOutOpen(&_device, _midiDevNum, NULL, 0, CALLBACK_NULL);

	if(rs != MMSYSERR_NOERROR)
		return;

	_open = true;
}

void MidiOut::closeDevice(){
	if(_open){
		midiOutReset(_device);
		midiOutClose(_device);
		_midiDevNum = -1;
		_open = false;
		_device = NULL;
	}
}

void MidiOut::shortMsg(DWORD data){

	if(!_open)
		return;

	midiOutShortMsg(_device, data);
}

void MidiOut::cc(int channel, int cc, int val){
	
	union{ DWORD dwData; BYTE bData[4]; } msg;

	msg.bData[0] = (BYTE)(0xb0 + channel);
	msg.bData[1] = (BYTE)cc;
	msg.bData[2] = (BYTE)val;
	msg.bData[3] = (BYTE)0;

	shortMsg(msg.dwData);
}

void MidiOut::setTone(int channel, int pc, int msb, int lsb){

	if(!_open)
		return;

	// Bank select
	cc(channel, 0, msb);
	cc(channel, 0x20, lsb);

	// Program change
	union{ DWORD dwData; BYTE bData[4]; } msg;
	msg.bData[2] = (BYTE)0;
	msg.bData[3] = (BYTE)0;

	msg.bData[0] = (BYTE)(0xc0 + channel);
	msg.bData[1] = (BYTE)pc;
	shortMsg(msg.dwData);
}

void MidiOut::sysex(string data){

	if(!_open)
		return;

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
}

bool MidiOut::isOpen(){
	return _open;
}
