#pragma once

#include<JuceHeader.h>

using namespace juce;


const Colour COLOR_BUTTON_HIGHLIGHTED	= Colour(30, 40, 45);
const Colour COLOR_BUTTON_DOWN			= Colour(10, 20, 25);

class CLookAndFeel : public LookAndFeel_V4 {
public:
	CLookAndFeel();
	
	void drawButtonBackground(Graphics&, Button&, const Colour& backgroundColour,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

	void drawButtonText(Graphics&, TextButton&,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};