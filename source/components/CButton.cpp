#include"CButton.h"

#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;

CButton::CButton(Controller& controller, const string& id, int x, int y) : ToggleButton(), midiOut_(controller.getMidiOut()) {
	setComponentID(id);
	addListener(this);

	// Correct button placement
	setBounds(x + 28, y - 10, 80, 80);
}

void CButton::buttonClicked(Button* button){

	// ID regex
	string id = button->getComponentID().toStdString();
	smatch matches;

	// Send MIDI CC out
	regex rgx("cc\\s+(\\d{1,2})ch(\\d{1,2})");

	if(regex_search(id, matches, rgx)){
		midiOut_.cc(std::stoi(matches.str(2)), std::stoi(matches.str(1)), button->getToggleState() ? 127 : 0);
		return;
	}

	// Send MIDI SysEx out
	rgx = regex("sysex\\s*(.+)");

	if(regex_search(id, matches, rgx))
		midiOut_.sysex((string)matches.str(1));
}
