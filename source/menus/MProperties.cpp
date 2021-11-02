#include "MProperties.h"

#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;


MProperties::MProperties(Controller& controller) : _controller(controller) {

	addLabel("Envelope", 20, 20, true);
	addSlider("Attack", "cc73", 10, 50);
	addSlider("Decay", "cc75", 90, 50);
	addSlider("Release", "cc72", 170, 50);

	addLabel("Vibrato", 350, 20, true);
	addSlider("Depth", "cc77", 340, 50);
	addSlider("Rate", "cc76", 420, 50);
	addSlider("Delay", "cc78", 500, 50);

	addLabel("Filter", 20, 220, true);
	addSlider("Cutoff", "cc74", 10, 250);
	addSlider("Resonance", "cc71", 90, 250);

	addLabel("Portamento", 240, 220, true);
	addButton("On/Off", "cc65", 230, 250);
	addSlider("Time", "cc5", 310, 250);

	addLabel("Modulation", 460, 220, true);
	addSlider("Amount", "cc1", 450, 250);

	addLabel("Effects", 20, 420, true);
	addSlider("Reverb", "cc91", 10, 450);
	addSlider("Chorus", "cc93", 90, 450);
}

MProperties::~MProperties(){

	for(auto i = _sldAttachments.begin(); i != _sldAttachments.end(); i++)
		delete* i;

	for(auto i = _btnAttachments.begin(); i != _btnAttachments.end(); i++)
		delete* i;

	for(auto i = _sliders.begin(); i != _sliders.end(); i++)
		delete* i;

	for(auto i = _buttons.begin(); i != _buttons.end(); i++)
		delete* i;

	for(auto i = _labels.begin(); i != _labels.end(); i++)
		delete* i;
}

void MProperties::updateChannel(){

	regex rgx("ch(\\d{1,2})");

	for(Slider* s : _sliders){

		// Find channel number in ID
		string id = s->getComponentID().toStdString();
		smatch matches;

		// Set visible if number in ID is equal to channel
		if(regex_search(id, matches, rgx))
			s->setVisible(_controller.getChannel() == std::stoi(matches.str(1)));
	}

	for(Button* b : _buttons){

		// Find channel number in ID
		string id = b->getComponentID().toStdString();
		smatch matches;

		// Set visible if number in ID is equal to channel
		if(regex_search(id, matches, rgx))
			b->setVisible(_controller.getChannel() == std::stoi(matches.str(1)));
	}
}


Label* MProperties::addSlider(const string& name, string id, int x, int y, bool toggle){

	for(int i = 0; i < 16; i++){

		string sid = id + " ch" + std::to_string(i);
		CSlider* s = new CSlider(_controller, sid, x, y);

		_sliders.push_back(s);
		addAndMakeVisible(s);

		_sldAttachments.push_back(new AudioProcessorValueTreeState::SliderAttachment(_controller.getParameters(), sid, *s));

		// Make slider an on/off switch
		if(toggle){
			s->setSliderStyle(Slider::LinearHorizontal);
			s->setRange(s->getMinimum(), 1, 1);

			auto b = s->getBounds();
			b.setX(b.getX() + 15);
			b.setWidth(50);
			s->setBounds(b);
		}
	}

	return addLabel(name, x, y + 80);
}

void MProperties::addButton(const string& name, const string& id, int x, int y){

	addLabel(name, x, y + 60);

	for(int i = 0; i < 16; i++){

		string bid = id + " ch" + std::to_string(i);
		CButton* b = new CButton(_controller, bid, x, y);

		_buttons.push_back(b);
		addAndMakeVisible(b);

		_btnAttachments.push_back(new AudioProcessorValueTreeState::ButtonAttachment(_controller.getParameters(), bid, *b));
	}
}

Label* MProperties::addLabel(const string& text, int x, int y, bool large){
	Label* l = new Label();
	l->setText(text, dontSendNotification);
	l->setFont(large ? 20.0f : 15.0f);
	l->setBounds(x, y, large ? 250 : 80, 25);

	if(!large)
		l->setJustificationType(Justification::centred);

	_labels.push_back(l);
	addAndMakeVisible(l);

	return l;
}
