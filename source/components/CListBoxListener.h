#pragma once

#include<JuceHeader.h>


class CListBoxListener{
public:
	virtual void listBoxChanged(juce::ListBox* lb, int row) = 0;
};