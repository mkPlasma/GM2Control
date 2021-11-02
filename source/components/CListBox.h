#pragma once

#include<JuceHeader.h>
#include"../Controller.h"
#include"CListBoxListener.h"
#include<string>
#include<vector>

using namespace juce;
using std::string;
using std::vector;


class CListBox : public ListBox, public ListBoxModel{

	vector<string> _contents;

	Controller& _controller;
	CListBoxListener* _listener;

	bool _forceRefresh;

public:
	CListBox(Controller& controller, string name);
	CListBox(Controller& controller, string name, vector<string> contents);

	int getNumRows() override;
	void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

	void setContents(vector<string> cont);
	void loadSavedRow();

	void setListener(CListBoxListener* l);
	void selectedRowsChanged(int lastRowSelected) override;
};