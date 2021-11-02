#pragma once

#include<JuceHeader.h>
#include"Controller.h"

using namespace juce;


class GM2ControlProcessor : public AudioProcessor{
public:
	GM2ControlProcessor();
	~GM2ControlProcessor() override;

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	Controller& getController();
	AudioProcessorValueTreeState& getParameters();

private:
	AudioProcessorValueTreeState _parameters;

	Controller _controller;

	bool _initialized;

	AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void initMidi();
};
