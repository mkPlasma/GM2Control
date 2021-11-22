#include "MToneList.h"

#include"../PluginEditor.h"


MToneList::MToneList(Controller& controller, GM2ControlEditor& editor) : _controller(controller), _editor(editor),
		_toneList(controller.getDevice().toneList),	_midiOut(controller.getMidiOut()), _channel(controller.getChannel()){

	// Tone listBox
	_lbTones = new CListBox(_controller, "tone" + std::to_string(_channel));
	_lbTones->setListener(this);
	_lbTones->setRowHeight(22);
	_lbTones->setBounds(240, 50, 390, 520);

	addAndMakeVisible(_lbTones);


	// Category listBox
	_lbCategories = new CListBox(_controller, "cat" + std::to_string(_channel), _toneList.getCategoryNames());
	_lbCategories->setListener(this);
	_lbCategories->setBounds(10, 50, 220, 520);

	addAndMakeVisible(_lbCategories);


	// Search bar
	_lSearch = new Label("toneSearch");
	_lSearch->setBounds(10, 10, 400, 30);
	_lSearch->setColour(Label::backgroundColourId, Colour((uint8)16, (uint8)16, (uint8)16, (float)1.0));
	_lSearch->setEditable(true);
	_lSearch->onTextChange = std::bind(&MToneList::updateSearch, this);

	addAndMakeVisible(_lSearch);


	// Category search button
	_btnSearchCats = new ToggleButton("Search category names");
	_btnSearchCats->setBounds(420, 10, 200, 30);

	addAndMakeVisible(_btnSearchCats);
}

MToneList::~MToneList(){
	delete _lbCategories;
	delete _lbTones;
	delete _lSearch;
	delete _btnSearchCats;
}

void MToneList::update(){

	if(_toneList.getTones().empty())
		return;

	// Set tones
	for(int i = 0; i < 16; i++){

		string iStr = std::to_string(i);

		// If tone is set, send MIDI and set proper name
		if((bool)_controller.getParameter("toneSet" + iStr)){

			try{
				Tone tone = _toneList.getTones().at(
					(int)_controller.getParameter("cat" + iStr)
				).tones.at((int)_controller.getParameter("tone" + iStr));

				_midiOut.setTone(i, tone.pc, tone.msb, tone.lsb);
				_editor.setChannelName(i, tone.name);
			}

			// Invalid indices, reset parameter values
			catch(std::out_of_range&){
				_controller.setParameter("toneSet" + iStr, false);
				_controller.setParameter("cat" + iStr, 0);
				_controller.setParameter("tone" + iStr, 0);
			}
		}
		else
			_editor.setChannelName(i, "-");
	}

	// Refresh categories
	_lbCategories->setContents(_toneList.getCategoryNames());
	_lbCategories->setRowHeight(max(min(510 / (int)_toneList.getTones().size(), 40), 25));

	// Select proper category
	updateChannel();
}

void MToneList::updateSearch(){

}

void MToneList::updateChannel(){
	_channel = _controller.getChannel();

	// Get saved selected row
	int row = (int)_controller.getParameter("cat" + std::to_string(_channel));

	// If not yet set, set to last row (blank instrument)
	if(!(bool)_controller.getParameter("toneSet" + std::to_string(_channel)))
		row = (int)_toneList.getTones().size() - 1;

	_lbCategories->selectRow(row);

	// Reload to make sure correct tone is selected
	listBoxChanged(_lbCategories, row);
}

void MToneList::listBoxChanged(ListBox* clb, int row){

	// Ignore deselected row
	if(row == -1)
		return;

	string channelStr = std::to_string(_channel);

	// Category changed
	if(clb == _lbCategories){
		_lbTones->setContents(_toneList.getToneNames(row));

		// If last category (blank instrument) is selected, select first tone
		if(row == _toneList.getTones().size() - 1){
			_lbTones->selectRow(0);
			_editor.setChannelName(_channel, _toneList.getTones()[row].tones[0].name);
			_controller.setParameter("toneSet" + channelStr, false);
			return;
		}

		// If selected tone is in this category, highlight it
		if(row == (int)_controller.getParameter("cat" + channelStr) &&
			(bool)_controller.getParameter("toneSet" + channelStr)){
			_lbTones->selectRow((int)_controller.getParameter("tone" + channelStr));
			return;
		}
		
		_lbTones->deselectAllRows();
		return;
	}

	// Tone changed and is not in blank category
	if(clb == _lbTones && _lbCategories->getLastRowSelected() != _toneList.getTones().size() - 1){

		Tone tone = _toneList.getTones()[_lbCategories->getLastRowSelected()].tones[row];

		_midiOut.setTone(_channel, tone.pc, tone.msb, tone.lsb);

		// Set channel button text
		_editor.setChannelName(_channel, tone.name);

		// Save category and tone
		_controller.setParameter("cat" + channelStr, _lbCategories->getLastRowSelected());
		_controller.setParameter("tone" + channelStr, row);
		_controller.setParameter("toneSet" + channelStr, true);

		return;
	}
}