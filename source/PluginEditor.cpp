#include"PluginEditor.h"

#include"components/CTabbedComponent.h"

using namespace juce;

#define CHANNEL_BTN_X		10
#define CHANNEL_BTN_Y		50
#define CHANNEL_BTN_SIZE	40


GM2ControlEditor::GM2ControlEditor(GM2ControlProcessor& p)
	: AudioProcessorEditor(&p), _processor(p), _controller(p.getController()){

	setSize(1000, 670);
	setResizable(false, false);
	_clf = new CLookAndFeel();


	// Device select button
	_btnDevice = new TextButton("Select device");
	_btnDevice->addListener(this);
	_btnDevice->setBounds(10, 10, 250, 30);
	addAndMakeVisible(_btnDevice);


	// Open MIDI button
	_btnMidi = new TextButton("Reopen MIDI");
	_btnMidi->addListener(this);
	_btnMidi->setBounds(740, 10, 120, 30);
	addAndMakeVisible(_btnMidi);


	// Reload button
	_btnReload = new TextButton("Reload files");
	_btnReload->addListener(this);
	_btnReload->setBounds(870, 10, 120, 30);
	addAndMakeVisible(_btnReload);
	

	// MIDI status light
	_lMidiStatus[0] = new Label("", "");
	_lMidiStatus[0]->setBounds(272, 21, 16, 12);

	// Outline
	_lMidiStatus[1] = new Label("", "");
	_lMidiStatus[1]->setBounds(270, 19, 20, 16);
	_lMidiStatus[1]->setColour(Label::backgroundColourId, Colours::black);

	addAndMakeVisible(_lMidiStatus[1]);
	addAndMakeVisible(_lMidiStatus[0]);


	// Tabs
	_tabs = new CTabbedComponent(TabbedButtonBar::TabsAtTop);
	_tabs->setTabBarDepth(30);
	_tabs->setCurrentTabIndex(0);
	_tabs->setBounds(350, 50, 640, 610);

	_mProperties = new MProperties(_controller);
	_mToneList = new MToneList(_controller, *this);

	_tabs->addTab("Properties", Colours::transparentBlack, _mProperties, false);
	_tabs->addTab("Tones", Colours::transparentBlack, _mToneList, false);

	_tabs->currentTabChanged(0, "");
	addAndMakeVisible(_tabs);


	// Channel buttons
	_btnChannels = new TextButton*[16];

	// Set selected channel
	int ch = (int)_controller.getParameter("channel");

	for(int i = 0; i < 16; i++){

		TextButton* btn = new TextButton((i < 9 ? "0" : "") + std::to_string(i + 1));
		btn->setLookAndFeel(_clf);

		btn->addListener(this);
		btn->setClickingTogglesState(true);
		btn->setRadioGroupId(1001);
		btn->setBounds(CHANNEL_BTN_X, CHANNEL_BTN_Y + i * (CHANNEL_BTN_SIZE - 2), 320, CHANNEL_BTN_SIZE);
		addAndMakeVisible(btn);

		if(i == ch)
			btn->setToggleState(true, dontSendNotification);

		_btnChannels[i] = btn;
	}


	_animator = new ComponentAnimator();

	// Note animation
	for(int i = 0; i < 16; i++){

		int x = CHANNEL_BTN_X + 272;
		int y = CHANNEL_BTN_Y + 14 + i * (CHANNEL_BTN_SIZE - 2);
		int w = 26;
		int h = 12;

		// Green (on)
		_noteAnim[0][i] = new Label("", "");
		_noteAnim[0][i]->setBounds(x, y, w, h);
		_noteAnim[0][i]->setColour(Label::backgroundColourId, Colours::lime);

		if(_controller.allNotesInChannelOff(i))
			_noteAnim[0][i]->setAlpha(0);

		// White (on, flash animation)
		_noteAnim[1][i] = new Label("", "");
		_noteAnim[1][i]->setBounds(x, y, w, h);
		_noteAnim[1][i]->setColour(Label::backgroundColourId, Colours::white);
		_noteAnim[1][i]->setAlpha(0);

		// Black (off)
		_noteAnim[2][i] = new Label("", "");
		_noteAnim[2][i]->setBounds(x, y, w, h);
		_noteAnim[2][i]->setColour(Label::backgroundColourId, Colours::black);

		// Outline
		_noteAnim[3][i] = new Label("", "");
		_noteAnim[3][i]->setBounds(x - 2, y - 2, w + 4, h + 4);
		_noteAnim[3][i]->setColour(Label::backgroundColourId, Colours::darkgrey);

		// Add bottom-top order
		addAndMakeVisible(_noteAnim[3][i]);
		addAndMakeVisible(_noteAnim[2][i]);
		addAndMakeVisible(_noteAnim[0][i]);
		addAndMakeVisible(_noteAnim[1][i]);
	}

	updateUI();
}

GM2ControlEditor::~GM2ControlEditor(){

	delete _btnReload;
	delete _btnMidi;
	delete _btnDevice;
	delete _lMidiStatus[0];
	delete _lMidiStatus[1];
	delete _tabs;
	delete _mProperties;
	delete _mToneList;
	delete _animator;

	for(int i = 0; i < 16; i++)
		delete _btnChannels[i];

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 16; j++)
			delete _noteAnim[i][j];

	setLookAndFeel(nullptr);
	delete _clf;
}

void GM2ControlEditor::paint(Graphics& g){
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void GM2ControlEditor::resized(){
}

void GM2ControlEditor::buttonClicked(Button* btn){

	// Preset button
	if(btn == _btnDevice){

		// Get file
		FileChooser fc("Select device preset file", File::getSpecialLocation(File::userHomeDirectory), "*.json");

		if(fc.browseForFileToOpen()){
			File file(fc.getResult());
			_controller.loadDevice(file.getFullPathName().toStdString());
			updateUI();
		}

		return;
	}

	// MIDI button
	if(btn == _btnMidi){
		MidiOut& midiOut = _controller.getMidiOut();

		// Close any open device then re-open
		midiOut.closeDevice();
		midiOut.openDevice();

		updateUI();
		return;
	}

	// Reload button
	if(btn == _btnReload){
		if(_controller.getDevice().loaded){
			_controller.loadDevice(_controller.getDevice().path);
			updateUI();
		}
		return;
	}


	// Channel buttons

	// Check if button is on
	if(!btn->getToggleState())
		return;

	// Find button clicked
	for(int i = 0; i < 16; i++){
		if(_btnChannels[i] == btn){

			// Select channel
			_controller.setChannel(i);

			// Update UI
			_mProperties->updateChannel();
			_mToneList->updateChannel();
			return;
		}
	}
}

void GM2ControlEditor::setChannelName(int channel, string name){
	_btnChannels[channel]->setButtonText((channel < 9 ? "0" : "") + std::to_string(channel + 1) + name);
}

void GM2ControlEditor::updateUI(){

	// Set device name
	_btnDevice->setButtonText(_controller.getDevice().loaded ? _controller.getDevice().name : "Select device");

	// Set MIDI status
	_lMidiStatus[0]->setColour(Label::backgroundColourId, _controller.getMidiOut().isOpen() ? Colours::limegreen : Colours::red);
	
	// Set channel
	_btnChannels[_controller.getChannel()]->setToggleState(true, dontSendNotification);

	// Update menus
	_mProperties->updateChannel();
	_mToneList->update();
}

void GM2ControlEditor::noteOn(int channel){
	_noteAnim[0][channel]->setAlpha(1);
	_noteAnim[1][channel]->setAlpha(1);
	animate(_noteAnim[0][channel], 0.75f, 200, 0.5);
	animate(_noteAnim[1][channel], 0.25f, 125, 0.5);
}

void GM2ControlEditor::noteOff(int channel){
	animate(_noteAnim[0][channel], 0, 200, 0.5);
	animate(_noteAnim[1][channel], 0, 125, 0.5);
}

void GM2ControlEditor::animate(Component* comp, float alpha, int time, double startSpeed){
	_animator->animateComponent(comp, comp->getBounds(), alpha, time, false, startSpeed, 1);
}
