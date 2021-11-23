#include"Controller.h"

#include<string>
#include<fstream>
#include<streambuf>
#include"json.hpp"
#include"PluginProcessor.h"
#include"PluginEditor.h"
#include"HexFunc.h"


Controller::Controller(GM2ControlProcessor& processor) : _processor(processor), _parameters(processor.getParameters()),
		_midiOut(*this), _channel(0) {

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

void Controller::setStatus(string status){
	if(_processor.getActiveEditor())
		((GM2ControlEditor*)_processor.getActiveEditor())->setStatus(status);
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

void Controller::setParameter(const string& parameter, int value){
	auto param = _parameters.getParameter(parameter);
	param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1((float)value));
}

void Controller::setParameter(const string& parameter, float value){
	auto param = _parameters.getParameter(parameter);
	param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(value));
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

	setParameter("channel", _channel);
}

int Controller::getChannel(){
	return _channel;
}
