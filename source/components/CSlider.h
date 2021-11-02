#pragma once

#include<JuceHeader.h>
#include"../Controller.h"
#include<string>

using namespace juce;
using std::string;


class CSlider : public Slider, public Slider::Listener {

	Controller& _controller;
	MidiOut& midiOut_;

	double dispMin_, dispMax_;
	vector<string> dispValueList_;
	bool dispOnOff_;

public:
	CSlider(Controller& controller, const string& id, int x, int y);

	void setDisplayRange(double dispMin, double dispMax);
	void setDispValueList(vector<string> dispValueList);
	void setOnOffDisplay(bool dispOff);

	String getTextFromValue(double v) override;
	double getValueFromText(const String& text) override;

	void sliderValueChanged(Slider* slider) override;
};