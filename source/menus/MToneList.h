#pragma once

#include<JuceHeader.h>
#include"../Controller.h"
#include"../components/CListBox.h"
#include"../components/CListBoxListener.h"
#include<string>
#include<vector>

using namespace juce;
using std::string;
using std::vector;

class GM2ControlEditor;


class MToneList : public Component, public CListBoxListener{

	Controller& _controller;
	GM2ControlEditor& _editor;

	const ToneList& _toneList;
	MidiOut& _midiOut;

	int _channel;

	CListBox* _lbCategories;
	CListBox* _lbTones;
	Label* _lSearch;
	ToggleButton* _btnSearchCats;

public:
	MToneList(Controller& contoller, GM2ControlEditor& editor);
	~MToneList();

	void update();
	void updateChannel();

	void updateSearch();

	void listBoxChanged(ListBox* lb, int row) override;
};