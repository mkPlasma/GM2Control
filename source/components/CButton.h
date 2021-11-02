#pragma once

#include<JuceHeader.h>
#include"../Controller.h"
#include<string>

using namespace juce;
using std::string;


class CButton : public ToggleButton, public ToggleButton::Listener {

	MidiOut& midiOut_;

public:
	CButton(Controller& controller, const string& id, int x, int y);

	void buttonClicked(Button* button) override;
};
