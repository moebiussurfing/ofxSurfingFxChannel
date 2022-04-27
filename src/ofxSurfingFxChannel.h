//
// Created by moebiussurfing on 2019-12-20.
//

#pragma once

#include "ofMain.h"

//----

// Optional:

#define USE_IM_GUI__OFX_SURFING_FX_CHANNEL // new gui
//#define USE_ofxGui // simpler gui could be activated with small modifications to remove ImGui dependency (TODO)

// To include some extra fx's: delay and echotrace
#define USE_FX_DELAYS	

// To enable ofxPresetsManager. (must add the addon to the project)
#define USE_ofxSurfingPresets // 1. presets manager (simpler and recommended)
//#define USE_ofxPresetsManager // 2. complex alterntive	



//---

//	TODO:
//	
//	add one extra fx: gpu lut?
//	reduce callbacks?

//---

#include "ofxSurfingHelpers.h"

// Fx Shaders
#include "ofxDotFrag.h"

// Gui
#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
#include "ofxSurfingImGui.h"
#endif

// Gui
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
	//--

public:
	//--------------------------------------------------------------
	ofxSurfingFxChannel() {
		// settings folder
		path_GLOBAL_Folder = "ofxSurfingFxChannel";

		// TODO: not required
		path_fileName_Session = "ofxSurfingFxChannel_Session.xml";

#ifndef USE_ofxPresetsManager
		path_fileName_Preset = "ofxSurfingFxChannel_Preset.xml"; // not used when using presetsManager
#endif

		ENABLE_FxChain.set("ENABLE", true);

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

public:
	void setup();
	void draw();
	void exit();
	void windowResized(int w, int h);

	ofParameter<int> window_W, window_H, window_X, window_Y;

	void drawGui();

private:

	void fboAllocate();

	void startup();

	//--

private:
	// main parameters. usualy to use outside the class as: control & preset
	ofParameterGroup parameters;
public:
	//--------------------------------------------------------------
	ofParameterGroup getParameters() {// control & preset
		return parameters;
	}
	//--------------------------------------------------------------
	ofParameterGroup getParameters_Preset() {// to use as preset
		return params_Preset;
	}

	//--

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
private:
	ofxSurfing_ImGui_Manager guiManager; // In MODE A ofxGui will be instatiated inside the class
	void drawImGui();

	ImGuiTreeNodeFlags fg0;
	ImGuiTreeNodeFlags fg1;
	ImGuiTreeNodeFlags fg2;
	ImGuiTreeNodeFlags fg3;
	ImGuiTreeNodeFlags fg4;
	ImGuiTreeNodeFlags fg5;
	ImGuiTreeNodeFlags fg6;

	// Styles
	void setupStyles();
	void ClearStyles();
#endif

	//--

private:
	void setup_FxChannel();
	void update_FxChannel();

	//--

	// gui workflow
	void refresh_Gui();
	void refresh_ofxGuiExtended_Check();//check if no fx enabled, then collapse all gui panels
	void refresh_Gui_minimize(bool bUseSolo = false);
	void refresh_Gui_Solos();

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	bool bOpenFrag1 = false;
	bool bOpenFrag2 = false;
	bool bOpenFrag3 = false;
	bool bOpenFrag4 = false;
	bool bOpenFrag5 = false;
	//bool bOpenFrag6 = false;
#endif

	//--

	// API 
	// feed the fx processor
public:
	void begin();
	void end();

	ofParameter<bool> ENABLE_FxChain; // main enabler toggle

	ofParameter<bool> bGui{ "SHOW GUI", true };

private:
	ofParameter<int> SELECT_Fx{ "SELECT FX", 1, 1, 3 };//select the fx to edit/show gui panel
	ofParameter<std::string> SELECT_Fx_Name{ "FX","" };//fx name
	ofParameter<bool> bSolo{ "Solo", false };//mute the other fx
	ofParameter<bool> bReset{ "Reset", false };//reset selected fx
	ofParameter<bool> bAll{ "All", false };
	ofParameter<bool> bNone{ "None", false };
	ofParameter<bool> bKeys{ "Keys", true };
	ofParameter<bool> bCollapse{ "Collapse", false };
	ofParameter<bool> bExpand{ "Expand", false };

#ifdef USE_ofxPresetsManager
	ofParameter<bool> bGui_Presets{ "SHOW PRESETS", true };
#endif
	
	bool bEnableGuiWorkflow = true;

	ofParameter<bool> ENABLE_Monochrome;
	ofParameter<bool> ENABLE_ThreeTones;
	ofParameter<bool> ENABLE_HSB;
#ifdef USE_FX_DELAYS
	ofParameter<bool> ENABLE_Delay;
	ofParameter<bool> ENABLE_Echotrace;
#endif

	ofParameterGroup params_Session;
	ofParameterGroup params_Subcontrol;
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

	// dot fx shaders

private:

	// basic fx
	ofx::dotfrag::Monochrome frag1;
	ofx::dotfrag::ThreeTones frag2;
	ofx::dotfrag::HSB frag3;

	// fx extra
#ifdef USE_FX_DELAYS
	ofx::dotfrag::Delay frag4;
	ofx::dotfrag::EchoTrace frag5;
	//ofx::dotfrag::Twist frag6;
#endif

public:
	//--------------------------------------------------------------
	void loadSettings() {
#ifndef USE_ofxPresetsManager
		ofxSurfingHelpers::loadGroup(params_Preset, path_GLOBAL_Folder + "/" + path_fileName_Preset);
#endif
	}

	//--

#ifdef USE_ofxGui
	ofxPanel gui;
#endif

	//--

	// presetsManager

#ifdef USE_ofxSurfingPresets
	ofxSurfingPresets presetsManager;
#endif

#ifdef USE_ofxPresetsManager
public:
	ofxPresetsManager presetsManager;
private:
	void setup_PresetsManager();
	ofParameterGroup params_PresetsManagerTools{ "> PRESETS" };
#endif

	//--

	// settings
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

#ifdef USE_ofxPresetsManager
	//--------------------------------------------------------------
	void setVisible_PresetClicker(bool b)
	{
		presetsManager.setVisible_PresetClicker(b);
	}
#endif

	//--------------------------------------------------------------
	void setKeysEnable(bool b) {
		bKeys = b;
	}
	//--------------------------------------------------------------
	void doReset() {
		bReset = true;
	}

	ofParameterGroup params_ControlExternal;
	//--------------------------------------------------------------
	ofParameterGroup getParamGroup_Control() {
		return params_ControlExternal;
	}

	//-

public:
	//--------------------------------------------------------------
	void setEnableKeys(bool b) {
		bKeys = b;
	}
	};
