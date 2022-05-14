//
// Created by moebiussurfing on 2019-12-20.
//

#pragma once

#include "ofMain.h"

//---

//	TODO:
//	
//	fix autopopulate more presets always..
//	fix groups gui workflow expand/collapse
//	add one extra fx: gpu lut?
//	reduce callbacks?

//---


// Optional:

#define USE_IM_GUI__OFX_SURFING_FX_CHANNEL // new gui
//#define USE_ofxGui // simpler gui could be activated with small modifications to remove ImGui dependency (TODO)

// To include some extra fx's: 
// delay and echotrace
#define USE_FX_DELAYS	

//-

// Enable a Presets Manager:

// Option 1. 
// Simpler and recommended!
#define USE_ofxSurfingPresets 

// Option 2. 
// A bit complex alternative.	
//#define USE_ofxPresetsManager


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

//-

// Optional:

// Presets manager

// Option 1
#ifdef USE_ofxSurfingPresets
#include "ofxSurfingPresets.h"
#endif

// Option 2
#ifdef USE_ofxPresetsManager
#include "ofxPresetsManager.h"
#endif

//-

class ofxSurfingFxChannel
{

public:
	void setup();
	void draw();
	void exit();
	void windowResized(int w, int h);

	void drawGui();
	void drawGui_User();//main controls

	//--

public:

	//--------------------------------------------------------------
	ofxSurfingFxChannel()
	{
		// settings folder
		path_GLOBAL_Folder = "ofxSurfingFxChannel";

		// TODO: not required
		path_fileName_Session = "ofxSurfingFxChannel_Session.xml";

#ifndef USE_ofxPresetsManager
		path_fileName_Preset = "ofxSurfingFxChannel_Preset.xml"; // not used when using presetsManager
#endif

		bENABLE_Fx.set("ENABLE", true);

		vflip = false;

		addKeysListeners();
	};

	//--------------------------------------------------------------
	~ofxSurfingFxChannel()
	{
		exit();
	};

private:

	// keys
	void keyPressed(ofKeyEventArgs &eventArgs);

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

	//--

#ifdef USE_ofxGui
private:
	// main parameters. usualy to use outside the class as: control & preset
	ofParameterGroup parameters;
public:
	//--------------------------------------------------------------
	ofParameterGroup getParameters() {// control & preset
		return parameters;
	}
#endif

public:
	//--------------------------------------------------------------
	ofParameterGroup getParameters_Preset() {// to use as preset
		return params_Preset;
	}

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
	void setup_FxChannel();
	void update_FxChannel();

	//--

	// Gui workflow

	void refresh_Gui_Solos();

	//-

	//TODO:
	// trig to implement workflow collapse / expand groups not working...

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

//	bool bOpenFrag1 = false;
//	bool bOpenFrag2 = false;
//	bool bOpenFrag3 = false;
//#ifdef USE_FX_DELAYS
//	bool bOpenFrag4 = false;
//	bool bOpenFrag5 = false;
//#endif

	//ImGuiTreeNodeFlags fg0;

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

	ofParameter<bool> bActive1_PRE{ "bActive1_PRE", false };
	ofParameter<bool> bActive2_PRE{ "bActive2_PRE", false };
	ofParameter<bool> bActive3_PRE{ "bActive3_PRE", false };
#ifdef USE_FX_DELAYS
	ofParameter<bool> bActive4_PRE{ "bActive4_PRE", false };
	ofParameter<bool> bActive5_PRE{ "bActive5_PRE", false };
#endif
	bool bSoloed = false;

	//--

	// API 
	// feed the fx processor
public:
	void begin();
	void end();

	ofParameter<bool> bENABLE_Fx; // main enabler/bypass toggle
	ofParameter<bool> bGui{ "FX CH", true }; // all gui
	ofParameter<bool> bGui_User{ "FX CH USER", true }; // user gui

private:
	ofParameter<int> SELECT_Fx{ "FX", 1, 1, 3 };//select the fx to edit/show gui panel
	ofParameter<std::string> SELECT_Fx_Name{ "_FX_","" };//fx name

	ofParameter<bool> bSolo{ "Solo", false };//mute the other fx
	ofParameter<bool> bReset{ "Reset", false };//reset selected fx
	ofParameter<bool> bAll{ "All", false };
	ofParameter<bool> bNone{ "None", false };
	ofParameter<bool> bKeys{ "Keys", true };
	ofParameter<bool> bGui_Edit{ "FX CH EDIT", false };

#ifdef USE_ofxPresetsManager
	ofParameter<bool> bGui_Presets{ "SHOW PRESETS", true };
#endif

	//ofParameter<bool> bCollapse{ "Collapse", false };
	//ofParameter<bool> bExpand{ "Expand", false };

	bool bEnableGuiWorkflow = true;

	//--

	ofParameter<bool> ENABLE_Monochrome;
	ofParameter<bool> ENABLE_ThreeTones;
	ofParameter<bool> ENABLE_HSB;
#ifdef USE_FX_DELAYS
	ofParameter<bool> ENABLE_Delay;
	ofParameter<bool> ENABLE_Echotrace;
#endif

	ofParameterGroup params_Session;
	ofParameterGroup params_Subcontrol;
	ofParameterGroup params_Subcontrol2;
	ofParameterGroup params_Editor;
	ofParameterGroup params_Preset;
	ofParameterGroup params_Control;

	// callbacks
	void Changed_params_Control(ofAbstractParameter &e);
	bool bDISABLECALLBACKS;

	//--

	// Fbo
private:
	ofFbo fbo_FxChain;
	bool vflip;
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

public:

#ifndef USE_ofxPresetsManager
	//--------------------------------------------------------------
	void loadSettings()
	{
		ofxSurfingHelpers::loadGroup(params_Preset, path_GLOBAL_Folder + "/" + path_fileName_Preset);
	}
#endif

	//--

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
	//--------------------------------------------------------------
	void doReset() {
		bReset = true;
	}


	//-

	//--------------------------------------------------------------
	void setEnableKeys(bool b) {
		bKeys = b;
	}

	//--------------------------------------------------------------
	void setKeysExtra(bool b) {
#ifdef USE_ofxSurfingPresets
		presetsManager.setEnableKeySpace(b); // enable space key bc used maybe by a play toggle
		presetsManager.setEnableKeysArrows(b); // enable arrows browse keys
#endif
	}

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
