#include "CTabbedComponent.h"

using namespace juce;


CTabbedComponent::CTabbedComponent(TabbedButtonBar::Orientation orientation) : TabbedComponent(orientation) {
	
}

void CTabbedComponent::currentTabChanged(int newCurrentTabIndex, const String&){

	for(int i = 0; i < getNumTabs(); i++)
		setTabBackgroundColour(i, TAB_COLOR_UNSELECTED);

	setTabBackgroundColour(newCurrentTabIndex, TAB_COLOR_SELECTED);
}