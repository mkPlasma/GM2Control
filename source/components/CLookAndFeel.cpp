#include "CLookAndFeel.h"

using namespace juce;


CLookAndFeel::CLookAndFeel(){

}


void CLookAndFeel::drawButtonBackground(Graphics& g, Button& b, const Colour& backgroundColour,
	bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown){

	auto buttonArea = b.getLocalBounds();

	// Button color
	if(b.getToggleState())
		g.setColour(backgroundColour);
	else if(shouldDrawButtonAsDown)
		g.setColour(COLOR_BUTTON_DOWN);
	else if(shouldDrawButtonAsHighlighted)
		g.setColour(COLOR_BUTTON_HIGHLIGHTED);
	else
		g.setColour(backgroundColour);
	g.fillRect(buttonArea);

	// Outline
	g.setColour(Colours::darkgrey);
	g.drawRect(buttonArea, 2);
}

void CLookAndFeel::drawButtonText(Graphics& g, TextButton& b,
	bool, bool){
	
	auto font = getTextButtonFont(b, b.getHeight());

	g.setFont(font);
	/*g.setColour(b.findColour(b.getToggleState() ? TextButton::textColourOnId
		: TextButton::textColourOffId)
		.withMultipliedAlpha(b.isEnabled() ? 1.0f : 0.5f));*/

	auto yIndent = jmin(4, b.proportionOfHeight(0.3f));
	auto cornerSize = jmin(b.getHeight(), b.getWidth()) / 2;
	auto fontHeight = roundToInt(font.getHeight() * 0.6f);
	auto leftIndent = jmin(fontHeight, 2 + cornerSize / (b.isConnectedOnLeft() ? 4 : 2));
	auto rightIndent = jmin(fontHeight, 2 + cornerSize / (b.isConnectedOnRight() ? 4 : 2));
	auto textWidth = b.getWidth() - leftIndent - rightIndent;

	if(textWidth > 0){
		// Draw numbers darker
		g.setColour(Colours::lightgrey);
		g.drawFittedText(b.getButtonText().substring(0, 2),
			leftIndent, yIndent, textWidth, b.getHeight() - yIndent * 2,
			Justification::left, 2);


		g.setColour(Colours::white);
		g.drawFittedText(b.getButtonText().substring(2),
			leftIndent + 25, yIndent, textWidth, b.getHeight() - yIndent * 2,
			Justification::left, 2);
	}
}