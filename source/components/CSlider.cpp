#include "CSlider.h"

#include<regex>

using std::regex;
using std::smatch;
using std::regex_search;


CSlider::CSlider(Controller& controller, const string& id, int x, int y) : Slider(), _controller(controller), midiOut_(controller.getMidiOut()) {
	setComponentID(id);
	addListener(this);

	setSliderStyle(RotaryHorizontalVerticalDrag);
	setTextBoxStyle(TextBoxBelow, false, 50, 20);

	setBounds(x, y, 80, 80);

	dispMin_ = -1;
	dispMax_ = -1;
	dispOnOff_ = false;
}

void CSlider::setDisplayRange(double dispMin, double dispMax){
	dispMin_ = dispMin;
	dispMax_ = dispMax;
}

void CSlider::setDispValueList(vector<string> dispValueList){
	dispValueList_ = dispValueList;
}

void CSlider::setOnOffDisplay(bool dispOnOff){
	dispOnOff_ = dispOnOff;
}

String CSlider::getTextFromValue(double v){

	if(dispOnOff_){
		// Display 'Off' instead of minimum value
		if(getValue() == getMinimum())
			return "Off";

		// Display 'On' if max is 1
		if(getValue() == getMaximum() && getMaximum() == 1)
			return "On";
	}

	// Normal display values
	if((dispMin_ == -1 || dispMax_ == -1) && dispValueList_.empty())
		return Slider::getTextFromValue(v);


	// Adjust minimum value for 'off' display
	double min = getMinimum();

	if(dispOnOff_)
		min++;

	// Custom min/max
	if(dispMin_ != -1){
		double value = dispMin_ + ((getValue() - min) / (getMaximum() - min)) * (dispMax_ - dispMin_);
		bool round = dispMin_ == (int)dispMin_ && dispMax_ == (int)dispMax_;

		return (round ? String(roundToInt(value)) : String(value, 2)) + getTextValueSuffix();
	}

	// Custom values list
	return String(dispValueList_.at((int)getValue() - min)) + getTextValueSuffix();
}

double CSlider::getValueFromText(const String& text){

	auto t = text.trim();

	// Remove suffix if present
	if(t.endsWith(getTextValueSuffix()))
		t = t.substring(0, t.length() - getTextValueSuffix().length());

	// Remove plus sign if present
	while(t.startsWithChar('+'))
		t = t.substring(1).trimStart();


	if(dispOnOff_){
		// Set to minimum if 'off' is typed
		if(t.equalsIgnoreCase("off"))
			return getMinimum();

		// Set to 1 if 'on' is typed and max is 1
		if(t.equalsIgnoreCase("on") && getMaximum() == 1)
			return 1;
	}


	// Item in list
	if(!dispValueList_.empty()){

		// Check for match first
		for(int i = 0; i < dispValueList_.size(); i++)
			if(t.equalsIgnoreCase(String(dispValueList_.at(i))))
				return i;

		// Check for partial match
		for(int i = 0; i < dispValueList_.size(); i++)
			if(String(dispValueList_.at(i)).startsWithIgnoreCase(t))
				return i;

		// Default to no change
		return getValue();
	}

	// Normal value
	double val = t.initialSectionContainingOnly("0123456789.,-").getDoubleValue();

	if((dispMin_ == -1 || dispMax_ == -1))
		return val;

	// Adjust minimum value for 'off' display
	double min = getMinimum();

	if(dispOnOff_)
		min++;

	return min + ((val - dispMin_) / (dispMax_ - dispMin_)) * (getMaximum() - min);
}

void CSlider::sliderValueChanged(Slider* slider){

	// ID regex
	string id = slider->getComponentID().toStdString();
	smatch matches;

	// Send MIDI CC out
	regex rgx("cc\\s*(\\d{1,2})\\s*ch(\\d{1,2})");

	if(regex_search(id, matches, rgx)){

		int val = (int)slider->getValue();

		// Adjust value for parameters that can be negative
		val += _controller.getParameters().getParameter(id)->getNormalisableRange().getRange().getStart() < 0 ? 64 : 0;

		midiOut_.cc(std::stoi(matches.str(2)), std::stoi(matches.str(1)), val);
		return;
	}

	// Send MIDI SysEx out
	rgx = regex("sysex\\s*(.+)");

	if(regex_search(id, matches, rgx))
		midiOut_.sysex((string)matches.str(1));
}