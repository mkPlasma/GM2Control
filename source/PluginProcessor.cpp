#include"PluginProcessor.h"

#include"PluginEditor.h"
#include<limits.h>

using namespace juce;
using std::make_unique;
using std::unique_ptr;


GM2ControlProcessor::GM2ControlProcessor() : AudioProcessor(BusesProperties()),
		_parameters(*this, nullptr, Identifier("GM2ControlParameters"), createParameterLayout()),
		_controller(_parameters), _initialized(false)
{
}

GM2ControlProcessor::~GM2ControlProcessor(){
}

AudioProcessorValueTreeState::ParameterLayout GM2ControlProcessor::createParameterLayout(){

	juce::AudioProcessorValueTreeState::ParameterLayout params;
	params.add(std::make_unique<AudioParameterInt>("channel", "Channel", 0, 16, 0));


	enum ParamType{INT, FLOAT, BOOL};

	struct Parameter{
		string id;
		string name;
		ParamType type;
		float v1;	// Min value
		float v2;	// Max value
		float v3;	// Default value
	};

	// Parameters to add for all 16 channels
	Parameter chParams[] = {
		{"cat",			"Tone Category",		INT, 0, 65536, 0},
		{"tone",		"Tone",					INT, 0, 65536, 0},
		{"toneSet",		"Tone set",				BOOL, 0, 0, false},

		{"cc73 ch",		"Envelope Attack",		INT, -64, 63, 0},
		{"cc75 ch",		"Envelope Decay",		INT, -64, 63, 0},
		{"cc72 ch",		"Envelope Release",		INT, -64, 63, 0},

		{"cc77 ch",		"Vibrato Depth",		INT, -64, 63, 0},
		{"cc76 ch",		"Vibrato Rate",			INT, -64, 63, 0},
		{"cc78 ch",		"Vibrato Delay",		INT, -64, 63, 0},

		{"cc74 ch",		"Filter Cutoff",		INT, -64, 63, 0},
		{"cc71 ch",		"Filter Resonance",		INT, -64, 63, 0},

		{"cc65 ch",		"Portamento",			BOOL, 0, 0, false},
		{"cc5 ch",		"Portamento Time",		INT, 0, 127, 0},

		{"cc1 ch",		"Modulation",			INT, 0, 127, 0},

		{"cc91 ch",		"Reverb",				INT, 0, 127, 0},
		{"cc93 ch",		"Chorus",				INT, 0, 127, 0},
	};

	// Add parameters
	for(int i = 0; i < 16; i++){
		string iStr = std::to_string(i);

		for(Parameter p : chParams){

			string id = p.id + iStr;
			string name = p.name + " " + iStr;

			switch(p.type){
			case INT:
				params.add(std::make_unique<AudioParameterInt>(id, name, p.v1, p.v2, p.v3));
				break;

			case FLOAT:
				params.add(std::make_unique<AudioParameterFloat>(id, name, p.v1, p.v2, p.v3));
				break;

			case BOOL:
				params.add(std::make_unique<AudioParameterBool>(id, name, p.v3));
				break;
			}
		}
	}
	
	return params;
}

const String GM2ControlProcessor::getName() const{
	return JucePlugin_Name;
}

bool GM2ControlProcessor::acceptsMidi() const{
	return true;
}

bool GM2ControlProcessor::producesMidi() const{
	return true;
}

bool GM2ControlProcessor::isMidiEffect() const{
	return false;
}

double GM2ControlProcessor::getTailLengthSeconds() const{
	return 0.0;
}

int GM2ControlProcessor::getNumPrograms(){
	return 1;
}

int GM2ControlProcessor::getCurrentProgram(){
	return 0;
}

void GM2ControlProcessor::setCurrentProgram(int){
}

const String GM2ControlProcessor::getProgramName(int){
	return {};
}

void GM2ControlProcessor::changeProgramName(int, const String&){
}

void GM2ControlProcessor::prepareToPlay(double, int){
}

void GM2ControlProcessor::releaseResources(){
}

bool GM2ControlProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const{
	if(layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
	 && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	if(layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;

	return true;
}

void GM2ControlProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages){

	// Audio not used
	buffer.clear();

	// Check MIDI device is open
	if(!_controller.getMidiOut().isOpen())
		return;

	// Initialize
	if(!_initialized)
		initMidi();


	for(auto meta : midiMessages){

		auto msg = meta.getMessage();

		// Short message, send back to MIDI out
		if(msg.getRawDataSize() == 3){

			// Send message
			_controller.getMidiOut().shortMsg(msg);


			// Note on/off animation
			if(msg.isNoteOn()){
				_controller.setNote(msg.getChannel() - 1, msg.getNoteNumber(), true);

				if(getActiveEditor() != nullptr)
					((GM2ControlEditor*)getActiveEditor())->noteOn(msg.getChannel() - 1);
			}
			else if(msg.isNoteOff()){
				_controller.setNote(msg.getChannel() - 1, msg.getNoteNumber(), false);

				if(getActiveEditor() != nullptr && _controller.allNotesInChannelOff(msg.getChannel() - 1))
					((GM2ControlEditor*)getActiveEditor())->noteOff(msg.getChannel() - 1);
			}
		}
	}

	// Clear input messages
	midiMessages.clear();

	// Send messages in MIDI buffer
	_controller.getMidiOut().sendMessagesInBuffer();
}

void GM2ControlProcessor::initMidi(){

	// CC parameters to initialize
	int ccList[] = {73, 75, 72, 77, 76, 78, 74, 71, 65, 5, 1, 91, 93};

	for(int ch = 0; ch < 16; ch++){

		string chStr = std::to_string(ch);

		// Send tone CC
		if((bool)_controller.getParameter("toneSet" + chStr)){
			int catIndex = (int)_controller.getParameter("cat" + chStr);
			int toneIndex = (int)_controller.getParameter("tone" + chStr);

			Tone tone = _controller.getDevice().toneList.getTones()[catIndex].tones[toneIndex];

			_controller.getMidiOut().setTone(ch, tone.pc, tone.msb, tone.lsb);
		}

		// Send CC
		for(int cc : ccList){

			// Get parameter value
			string id = "cc" + std::to_string(cc) + " ch" + chStr;
			int val = (int)_controller.getParameter(id);

			// Adjust value for parameters that can be negative
			val += _controller.getParameters().getParameter(id)->getNormalisableRange().getRange().getStart() < 0 ? 64 : 0;

			// Send MIDI
			_controller.getMidiOut().cc(ch, cc, val);
		}
	}

	_initialized = true;
}

bool GM2ControlProcessor::hasEditor() const{
	return true;
}

AudioProcessorEditor* GM2ControlProcessor::createEditor(){
	return new GM2ControlEditor(*this);
}

void GM2ControlProcessor::getStateInformation(MemoryBlock& destData){
	
	// Parameters
	auto state = _parameters.copyState();
	unique_ptr<XmlElement> xml(state.createXml());

	// Device preset file path
	XmlElement* xmlDFP = new XmlElement("deviceFilePath");
	xmlDFP->setAttribute("value", _controller.getDevice().path);
	
	xml->addChildElement(xmlDFP);

	copyXmlToBinary(*xml, destData);
}

void GM2ControlProcessor::setStateInformation(const void* data, int sizeInBytes){

	unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

	if(xml.get() != nullptr){

		// Parameters
		if(xml->hasTagName(_parameters.state.getType())){
			_parameters.replaceState(ValueTree::fromXml(*xml));

			// Device preset file path
			XmlElement* xmlDFP = xml->getChildByName("deviceFilePath");
			if(xmlDFP)
				_controller.loadDevice(xmlDFP->getStringAttribute("value").toStdString());

			// Set channel
			_controller.setChannel((int)_controller.getParameter("channel"));
		}

		// Update editor UI
		if(getActiveEditor() != nullptr)
			((GM2ControlEditor*)getActiveEditor())->updateUI();
	}
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter(){
	return new GM2ControlProcessor();
}

Controller& GM2ControlProcessor::getController(){
	return _controller;
}

AudioProcessorValueTreeState& GM2ControlProcessor::getParameters(){
	return _parameters;
}
