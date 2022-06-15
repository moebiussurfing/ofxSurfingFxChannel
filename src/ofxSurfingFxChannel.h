//
// Created by moebiussurfing on 2019-12-20.
//

#pragma once

#include "ofMain.h"

//----

/*
 
	TODO:

	+ fix auto populate more presets always..
	+ fix groups GUI workflow expand/collapse on ImGui.
	+ add one extra FX: GPU LUT? Overlay?
	+ presets smooth don't works here.

*/

//----


// OPTIONAL:

//-

// 1. To include some extra FX's: 

// delay and echo trace
#define USE_FX_DELAYS	

//-

// 2. Pick a Gui

//#define USE_ofxGui // Simpler GUI could be activated to remove ImGui dependency.
#define USE_IM_GUI__OFX_SURFING_FX_CHANNEL // Powerful ImGui based. Required to use Presets Manager modes!

//-

// 3. Enable a Presets Manager:

// Option 1. 
// Simpler and recommended!
//#define USE_ofxSurfingPresets 

// Option 2. 
// A bit complex alternative.	
//#define USE_ofxPresetsManager

//-


//----


// Don't allow the two GUIs together!
// You just pick only one.
#ifdef USE_ofxGui
#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
#undef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
#endif
#endif

//----

#include "ofxSurfingHelpers.h"

// Fx Shaders
#include "ofxDotFrag.h"

// Gui
#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
#include "ofxSurfingImGui.h"
#endif

// Gui //-> To disable all ImGui and enable ofxGui maybe you need to do some work...
#ifdef USE_ofxGui
#include "ofxGui.h"
#include "ofxSurfing_ofxGui.h"
#endif

//--

// Optional:

// Presets Manager

// Option 1
#ifdef USE_ofxSurfingPresets
#include "ofxSurfingPresets.h"
#endif

// Option 2
#ifdef USE_ofxPresetsManager
#include "ofxPresetsManager.h"
#endif

//--

class ofxSurfingFxChannel
{

public:

	ofxSurfingFxChannel();
	~ofxSurfingFxChannel();

	//--

public:

	void setup();
	void draw();
	void drawGui();
	void windowResized(int w, int h);

	//--

private:

	void setup_Params();
	void setupGui();
	void update_FxChannel();
	void exit();

	// keys
	void keyPressed(ofKeyEventArgs& eventArgs);

	//--------------------------------------------------------------
	void addKeysListeners()
	{
		ofLogNotice(__FUNCTION__);
		ofAddListener(ofEvents().keyPressed, this, &ofxSurfingFxChannel::keyPressed);
	};

	//--------------------------------------------------------------
	void removeKeysListeners()
	{
		ofLogNotice(__FUNCTION__);
		ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingFxChannel::keyPressed);
	};

	//--

private:

	ofParameter<int> window_W, window_H, window_X, window_Y;

	void fboAllocate();

	void startup();
	void startupDelayed();

	//--

private:

	void refresh_GuiMinimize();
	void refresh_GuiWorkflow();

	void refresh_FxName();
	bool bFlagRefresh = false;

	//--

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

private:

	ofxSurfing_ImGui_Manager guiManager;

	void drawImGui();

	// Styles
	void setupStyles();
	void ClearStyles();

#endif

	//--

private:

	//--

	// Gui workflow

	//--

	//TODO:
	// trig to implement workflow collapse / expand groups not working...

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	ImGuiTreeNodeFlags fgT1;
	ImGuiTreeNodeFlags fgT2;
	ImGuiTreeNodeFlags fgT3;
	ImGuiTreeNodeFlags fgT4;
#ifdef USE_FX_DELAYS
	ImGuiTreeNodeFlags fgT5;
	ImGuiTreeNodeFlags fgT6;
#endif

	SurfingImGuiTypesGroups fgG1;
	SurfingImGuiTypesGroups fgG2;
	SurfingImGuiTypesGroups fgG3;
#ifdef USE_FX_DELAYS
	SurfingImGuiTypesGroups fgG4;
	SurfingImGuiTypesGroups fgG5;
#endif

#endif

	//--

	ofParameter<bool> ENABLE_Monochrome_PRE{ "ENABLE_Monochrome_PRE", false };
	ofParameter<bool> ENABLE_ThreeTones_PRE{ "ENABLE_ThreeTones_PRE", false };
	ofParameter<bool> ENABLE_HSB_PRE{ "ENABLE_HSB_PRE", false };

#ifdef USE_FX_DELAYS
	ofParameter<bool> ENABLE_Delay_PRE{ "ENABLE_Delay_PRE", false };
	ofParameter<bool> ENABLE_Echotrace_PRE{ "ENABLE_Echotrace_PRE", false };
#endif

	bool bSoloed = false;

	//--

	// API 
	// feed the fx processor

public:

	void begin();
	void end();

	//--

	void doReset();

public:

	ofParameter<bool> bGui{ "FX CH", true }; // all gui
	ofParameter<bool> bGui_User{ "FX CH USER", true }; // user gui
	ofParameter<bool> bGui_Edit{ "FX CH EDIT", false };

	ofParameter<bool> bEnable_Fx; // main enabler/bypass toggle
	ofParameter<int> indexFx{ "FX", 1, 1, 3 };//select the fx to edit/show gui panel

private:

	ofParameter<std::string> indexFx_Name{ "Name","_" };//fx name

	ofParameter<bool> bSolo{ "Solo", false };//mute the other fx
	ofParameter<bool> bReset{ "Reset", false };//reset selected fx
	ofParameter<bool> bAll{ "All", false };
	ofParameter<bool> bNone{ "None", false };
	ofParameter<bool> bKeys{ "Keys", true };
	ofParameter<bool> bEnableGuiWorkflow{ "GuiWorkflow", true };

#ifdef USE_ofxPresetsManager
	ofParameter<bool> bGui_Presets{ "SHOW PRESETS", true };
#endif

	//--

	ofParameter<bool> ENABLE_Monochrome;
	ofParameter<bool> ENABLE_ThreeTones;
	ofParameter<bool> ENABLE_HSB;

#ifdef USE_FX_DELAYS
	ofParameter<bool> ENABLE_Delay;
	ofParameter<bool> ENABLE_Echotrace;
#endif

	ofParameterGroup params_Session;

public:

	ofParameterGroup params_Preview;
	ofParameterGroup params_Enablers;
	ofParameterGroup params_Preset;
	ofParameterGroup params_Control;

	//--

public:

	//--------------------------------------------------------------
	ofParameterGroup& getParameters_Preview() { // params previews
		return params_Preview;
	}
	//--------------------------------------------------------------
	ofParameterGroup& getParameters_Enablers() { // enablers params
		return params_Enablers;
	}
	//--------------------------------------------------------------
	ofParameterGroup& getParameters_Preset() { // to use as preset
		return params_Preset;
	}
	//--------------------------------------------------------------
	ofParameterGroup& getParameters_Control() { // all the controls
		return params_Control;
	}

	//--

private:

	// callbacks
	void Changed(ofAbstractParameter& e);
	bool bDISABLE_CALLBACKS = true;

	//--

	// Fbo

//protected:
private:

	ofFbo fbo_FxChain;

private:

	bool vflip = false;

public:

	void setVflip(bool b) {
		vflip = b;
	}

	//--

	// Dot Fx shaders

private:

	// Basic Fx
	ofx::dotfrag::Monochrome frag1;
	ofx::dotfrag::ThreeTones frag2;
	ofx::dotfrag::HSB frag3;

	// Fx Extra
#ifdef USE_FX_DELAYS
	ofx::dotfrag::Delay frag4;
	ofx::dotfrag::EchoTrace frag5;
	//ofx::dotfrag::Twist frag6;
#endif

	//--

private:

#ifdef USE_ofxGui

	ofxPanel gui;

#endif

	//--

	// Presets Manager

	// Option 1

#ifdef USE_ofxSurfingPresets

	ofxSurfingPresets presetsManager;

#endif

	//-

	// Option 2

#ifdef USE_ofxPresetsManager

public:

	ofxPresetsManager presetsManager;

private:

	void setup_PresetsManager();
	ofParameterGroup params_PresetsManagerTools{ "> PRESETS" };

#endif

	//--

	// Path settings

private:

	std::string path_GLOBAL_Folder;
	std::string path_fileName_Session;

#ifndef USE_ofxPresetsManager

	std::string path_fileName_Preset;

#endif

	//--

#ifdef USE_ofxGui

private:

	ofParameter<glm::vec2> position_Gui;

#endif

	//----

public:

	//--------------------------------------------------------------
	void setPath_GlobalFolder(std::string folder)
	{
		ofLogNotice(__FUNCTION__) << folder;
		path_GLOBAL_Folder = folder;
		ofxSurfingHelpers::CheckFolder(folder);
	}

	//--

#ifdef USE_ofxGui

	//--------------------------------------------------------------
	glm::vec2 getGuiPosition()
	{
		position_Gui = glm::vec2(gui.getPosition().x, gui.getPosition().y);

		return position_Gui.get();
	}
	//--------------------------------------------------------------
	void setGuiPosition(int x, int y) {
		setGuiPosition(glm::vec2(x, y));
	}
	//--------------------------------------------------------------
	void setGuiPosition(glm::vec2 pos) {
		position_Gui = pos;

		gui.setPosition(position_Gui.get().x, position_Gui.get().y);
	}
	//--------------------------------------------------------------
	float getGuiWidth()
	{
		float _gwidth;
		_gwidth = gui.getShape().getWidth();
		return _gwidth;
	}

#endif

	//--

	//--------------------------------------------------------------
	void setVisibleGui(bool b) {
		bGui = b;

#ifdef USE_ofxPresetsManager

		if (bGui_Presets && bGui) setVisible_PresetClicker(true);
		else if (!bGui) setVisible_PresetClicker(false);

#endif
}

	//--------------------------------------------------------------
	void setKeysEnable(bool b) {
		bKeys = b;
	}

	//-

	//--------------------------------------------------------------
	void setEnableKeys(bool b) {
		bKeys = b;
	}

	//--------------------------------------------------------------
	void setKeysExtra(bool b)
	{
#ifdef USE_ofxSurfingPresets

		presetsManager.setEnableKeySpace(b); // enable space key bc used maybe by a play toggle
		presetsManager.setEnableKeysArrows(b); // enable arrows browse keys

#endif
	}

	//--

#ifdef USE_ofxPresetsManager

	//--------------------------------------------------------------
	void setVisible_PresetClicker(bool b)
	{
		presetsManager.setVisible_PresetClicker(b);
	}

	ofParameterGroup params_ControlExternal;
	//--------------------------------------------------------------
	ofParameterGroup getParamGroup_Control() {
		return params_ControlExternal;
	}

#endif

};
