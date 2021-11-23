#pragma once

#include<JuceHeader.h>
#include"PluginProcessor.h"
#include"components/CLookAndFeel.h"
#include"menus/MProperties.h"
#include"menus/MToneList.h"

using namespace juce;
using std::string;


class GM2ControlEditor : public AudioProcessorEditor, public TextButton::Listener{
public:
	GM2ControlEditor(GM2ControlProcessor& p);
	~GM2ControlEditor() override;

	void paint(Graphics& g) override;


	void buttonClicked(Button* btn) override;

	void mouseDown(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;

	void setChannelName(int channel, string name);

	void updateUI();

	void setStatus(string status);

	// Channel note animation
	void noteOn(int channel);
	void noteOff(int channel);

private:
	GM2ControlProcessor& _processor;
	Controller& _controller;

	CLookAndFeel* _clf;

	// Default channel button color
	Colour _btnColor;

	// Select file for device preset
	TextButton* _btnDevice;

	// Open MIDI device
	TextButton* _btnMidi;

	// Reload files/UI
	TextButton* _btnReload;

	// MIDI status label
	Label* _lMidiStatus[2];

	// Status label
	Label* _lStatus[2];

	// Menus
	TabbedComponent* _tabs;
	MProperties* _mProperties;
	MToneList* _mToneList;

	// Channel selector buttons
	TextButton** _btnChannels;

	// For note on/off animation
	ComponentAnimator* _animator;
	//bool _note[16][128];

	// 0 - green (on)
	// 1 - white (on)
	// 2 - black (off)
	// 3 - outline
	Label* _noteAnim[4][16];


	// Settings drag and drop
	Label* _lDragDropText;
	int _dragDropChannel;


	int getChannelButtonAtPos(int x, int y);

	inline void animate(Component* comp, float alpha, int time, double startSpeed);
};
