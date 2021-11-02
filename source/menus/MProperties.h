#pragma once

#include<../JuceLibraryCode/JuceHeader.h>
#include"../Controller.h"
#include"../components/CSlider.h"
#include"../components/CButton.h"
#include<string>
#include<vector>

using namespace juce;
using std::string;
using std::vector;


class MProperties : public Component{

	Controller& _controller;
	
	vector<CSlider*> _sliders;
	vector<CButton*> _buttons;
	vector<AudioProcessorValueTreeState::SliderAttachment*> _sldAttachments;
	vector<AudioProcessorValueTreeState::ButtonAttachment*> _btnAttachments;
	vector<Label*> _labels;

	Label* addSlider(const string& name, string id, int x, int y, bool toggle = false);
	void addButton(const string& name, const string& id, int x, int y);
	Label* addLabel(const string& text, int x, int y, bool large = false);

public:
	MProperties(Controller& controller);
	~MProperties();

	void updateChannel();
};
