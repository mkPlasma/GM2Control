#include"Controller.h"

#include<string>
#include<fstream>
#include<streambuf>
#include"json.hpp"
#include"HexFunc.h"


Controller::Controller(AudioProcessorValueTreeState& parameters) : _midiOut(*this), _parameters(parameters), _channel(0) {

	for(int i = 0; i < 16; i++)
		for(int j = 0; j < 128; j++)
			_noteOn[i][j] = false;
}

void Controller::loadDevice(const string& path){

	_device.loaded = false;
	_device.path = path;

	// Open file
	std::ifstream in(path);

	// Check file exists
	if(!in.good()){
		in.close();
		return;
	}

	nlohmann::json deviceJson = nlohmann::json::parse(string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()));
	in.close();

	// Get values
	try{
		_device.name				= deviceJson.at("name").get<string>();
		_device.midiDeviceName		= deviceJson.at("midiDeviceName").get<string>();
		_device.toneListPath		= deviceJson.at("toneListPath").get<string>();
		_device.sysexHeaderByte1	= hexStringToByte(deviceJson.at("sysexHeaderByte1").get<string>());
		_device.sysexHeaderByte2	= hexStringToByte(deviceJson.at("sysexHeaderByte2").get<string>());
	}
	catch(const nlohmann::json::out_of_range&){
		return;
	}
	
	// Check values
	if(_device.name.empty()
		|| _device.midiDeviceName.empty()
		|| _device.toneListPath.empty()
		|| _device.sysexHeaderByte1 < 0
		|| _device.sysexHeaderByte1 > 127
		|| _device.sysexHeaderByte2 < 0
		|| _device.sysexHeaderByte2 > 127)
		return;


	// Get tone list file from device file directory
	if(_device.path[2] == '\\')
		_device.toneListPath = _device.path.substr(0, _device.path.find_last_of('\\') + 1) + _device.toneListPath;
	else
		_device.toneListPath = _device.path.substr(0, _device.path.find_last_of('/') + 1) + _device.toneListPath;

	// Load tones, return if failed
	if(!_device.toneList.load(_device.toneListPath))
		return;

	// Loaded successfully
	_device.loaded = true;

	// Close any currently open device and open new one
	_midiOut.closeDevice();
	_midiOut.openDevice();
}

void Controller::setNote(int channel, int note, bool on){
	_noteOn[channel][note] = on;
}

bool Controller::allNotesInChannelOff(int channel){

	for(int i = 0; i < 128; i++)
		if(_noteOn[channel][i])
			return false;

	return true;
}

void Controller::setParameter(const string& parameter, float value){
	_parameters.getParameter(parameter)->setValueNotifyingHost(value);
}

float Controller::getParameter(const string& parameter){
	auto v = _parameters.getRawParameterValue(parameter);
	if(v)
		return *(float*)v;
	else
		return 0;
}

const Device& Controller::getDevice(){
	return _device;
}

MidiOut& Controller::getMidiOut(){
	return _midiOut;
}

AudioProcessorValueTreeState& Controller::getParameters(){
	return _parameters;
}

void Controller::setChannel(int channel){
	_channel = channel;

	if(_channel < 0 || _channel > 16)
		_channel = 0;

	setParameter("channel", _channel / 16.0f);
}

int Controller::getChannel(){
	return _channel;
}
