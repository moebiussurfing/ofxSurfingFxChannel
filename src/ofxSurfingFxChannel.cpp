#include "ofxSurfingFxChannel.h"

//--------------------------------------------------------------
ofxSurfingFxChannel::ofxSurfingFxChannel()
{
	// settings folder
	path_GLOBAL_Folder = "ofxSurfingFxChannel";

	// TODO: not required
	path_fileName_Session = "ofxSurfingFxChannel_Session.json";

#ifndef USE_ofxPresetsManager
	path_fileName_Preset = "ofxSurfingFxChannel_Preset.json"; // not used when using presetsManager
#endif

	addKeysListeners();
}

//--------------------------------------------------------------
ofxSurfingFxChannel::~ofxSurfingFxChannel()
{
	// callback
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSurfingFxChannel::Changed);

	removeKeysListeners();

	exit();
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::doReset()
{
	// reset all fx
	{
		if (bSolo.get()) bSolo = false;

		frag1.active = false;
		frag1.low = 0;
		frag1.high = 1;

		frag2.active = false;
		frag2.mix = 1;
		frag2.thresholds[0] = 0;
		frag2.thresholds[1] = 0.5f;
		frag2.fade = 0.3f;
		frag2.colors[0] = 0;
		frag2.colors[1] = 128;
		frag2.colors[2] = 255;

		frag3.active = true;
		frag3.hue = 0;
		frag3.brightness = 0;
		frag3.contrast = 1;
		frag3.saturation = 1;

#ifdef USE_FX_DELAYS

		frag4.active = false;
		frag4.feedback = 0;
		frag4.blendmode = 0;

		frag5.active = false;
		frag5.gain = 0.7f;
		frag5.threshold = 0.55f;
		frag5.invert = false;
		frag5.hardCutoff = false;

#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::fboAllocate()
{
	window_W = ofGetWidth();
	window_H = ofGetHeight();

	//-

	bool bArbPRE = ofGetUsingArbTex();
	ofDisableArbTex();
	{
		ofFbo::Settings fboSettings;
		fboSettings.width = window_W;
		fboSettings.height = window_H;
		fboSettings.internalformat = GL_RGBA;

		//TODO: check if we can add 3d compatibility..
		fboSettings.textureTarget = GL_TEXTURE_2D;

		fbo_FxChain.allocate(fboSettings);

		//TODO:
		// clear
		fbo_FxChain.begin();
		ofClear(0, 0, 0, 255);
		fbo_FxChain.end();

		//-

		// Fx
		frag1.allocate(fbo_FxChain);
		frag2.allocate(fbo_FxChain);
		frag3.allocate(fbo_FxChain);

#ifdef USE_FX_DELAYS
		frag4.allocate(fbo_FxChain);
		frag5.allocate(fbo_FxChain);
		//frag6.allocate(fbo_FxChain);
#endif
	}
	if (bArbPRE) ofEnableArbTex();
	else ofDisableArbTex();
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::setupGui()
{
	ofLogNotice(__FUNCTION__);

	//-

#ifdef USE_ofxGui

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	gui.setup("FX CH");
	gui.add(params_Control);

#endif

	//----

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	guiManager.setup();

	setupStyles();

#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::setup()
{
	ofLogNotice(__FUNCTION__);

	fboAllocate();

	//--

	setup_Params();

	setupGui();

	//--

	// Startup 
	startup();
}

//TODO:
//--------------------------------------------------------------
void ofxSurfingFxChannel::startupDelayed()
{
	ofLogNotice(__FUNCTION__);
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::startup()
{
	ofLogNotice(__FUNCTION__);

	//-

	bDISABLE_CALLBACKS = false;

	//-

	// Not required when handled by the presets manager
#ifndef USE_ofxPresetsManager
	ofxSurfingHelpers::loadGroup(params_Preset, path_GLOBAL_Folder + "/" + path_fileName_Preset);
#endif

	// Load Session Settings
	ofxSurfingHelpers::loadGroup(params_Session, path_GLOBAL_Folder + "/" + path_fileName_Session);

	//--

#ifdef USE_ofxGui
	gui.setPosition(position_Gui->x, position_Gui->y);
#endif

	bFlagRefresh = true;

	//-

	// Refresh Gui workflow

	refresh_GuiMinimize();

	//--
	 
	//TODO:
	//bug crashing ofx SurfingFxPost
	pg.setName("fxparams");
	pg.add(indexFx);
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::setup_Params()
{
	// Parameters

	bEnable_Fx.set("ENABLE", true);

	//--

	// Customize names to tweak GUI labels

	frag1.parameters.setName("FX1 " + frag1.parameters.getName());
	frag2.parameters.setName("FX2 " + frag2.parameters.getName());
	frag3.parameters.setName("FX3 " + frag3.parameters.getName());

#ifdef USE_FX_DELAYS
	frag4.parameters.setName("FX4 " + frag4.parameters.getName());
	frag5.parameters.setName("FX5 " + frag5.parameters.getName());
#endif

	//--

	// Gui layout

#ifdef USE_ofxGui

	position_Gui.set("GUI POSITION",
		glm::vec2(10, 10),
		glm::vec2(0, 0),
		glm::vec2(1920, 1080)
	);

#endif

	//--

	// Gui

	// 1. Preview controls

	params_Preview.setName("PREVIEW");
	params_Preview.add(indexFx);
	params_Preview.add(indexFx_Name);
	params_Preview.add(bSolo);
	params_Preview.add(bReset);
	params_Preview.add(bAll);
	params_Preview.add(bNone);
	params_Preview.add(bEnableGuiWorkflow);
	params_Preview.add(bKeys);

	//--

	// 2. Fx Params

	// Renames

	frag1.active.setName("MONOCHROME");
	frag2.active.setName("THREETONES");
	frag3.active.setName("HSB");
#ifdef USE_FX_DELAYS
	frag4.active.setName("DELAY");
	frag5.active.setName("ECHOTRACE");
#endif

	//--

	// Linked Enablers

	ENABLE_Monochrome.makeReferenceTo(frag1.active);
	ENABLE_ThreeTones.makeReferenceTo(frag2.active);
	ENABLE_HSB.makeReferenceTo(frag3.active);
#ifdef USE_FX_DELAYS
	ENABLE_Delay.makeReferenceTo(frag4.active);
	ENABLE_Echotrace.makeReferenceTo(frag5.active);
#endif

	//--

	// Queue Params

	params_Enablers.setName("ENABLERS");
	params_Enablers.add(bEnable_Fx);
	params_Enablers.add(ENABLE_Monochrome);
	params_Enablers.add(ENABLE_ThreeTones);
	params_Enablers.add(ENABLE_HSB);

#ifdef USE_FX_DELAYS
	params_Enablers.add(ENABLE_Delay);
	params_Enablers.add(ENABLE_Echotrace);
#endif

	//--

	// To use callbacks only

	params_Control.setName("FX CHANNEL");
	params_Control.add(params_Preview);
	params_Control.add(params_Enablers);
	params_Control.add(params_Preset);

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	params_Control.add(guiManager.bMinimize);
#endif

#ifdef USE_ofxPresetsManager
	params_PresetsManagerTools.add(presetsManager.getParamsControls());
	// to use callbacks only
	params_Control.add(params_PresetsManagerTools);
	params_Control.add(bGui_Presets);
#endif

	//--

	// Exclude

	bReset.setSerializable(false);
	bAll.setSerializable(false);
	bNone.setSerializable(false);
	indexFx_Name.setSerializable(false);
	//indexFx.setSerializable(false);
	//bSolo.setSerializable(false);

	//--

	// Presets Params

	params_Preset.setName("PRESET");
	params_Preset.add(frag1.parameters);
	params_Preset.add(frag2.parameters);
	params_Preset.add(frag3.parameters);

#ifdef USE_FX_DELAYS
	params_Preset.add(frag4.parameters);
	params_Preset.add(frag5.parameters);
	//params_Preset.add(frag6.parameters);
	indexFx.setMax(5);
#endif

	//--

	// Presets Manager

	// Handles fx presets! (with all the settings)

#ifdef USE_ofxSurfingPresets
	presetsManager.setPathPresets(path_GLOBAL_Folder + "/" + "Presets");

	// Reduce default amount of presets
	presetsManager.setMaxPresetsAmount(4);

	//presetsManager.setName("myPlayer");

	// Add params
	presetsManager.addGroup(params_Preset);

	// Some preferences for this situation: being minimal
	presetsManager.bGui_Global.setName("PRESETS");
	presetsManager.bGui.setName("Fx Presets");
	presetsManager.bGui = true;
	presetsManager.bGui_Editor = false;
	presetsManager.bGui_Parameters = false;
	presetsManager.bGui_ClickerFloating = false;
	presetsManager.bGui_ClickerSimple = true;
	presetsManager.bMinimize = true;
	presetsManager.setPlayerPlay(false);
	presetsManager.setMaxPresetsAmountPerRowClickerFloat(2);
	presetsManager.setMaxPresetsAmountPerRowClickerMini(2);
	presetsManager.bKeys.makeReferenceTo(bKeys);//link toggles
#endif

#ifdef USE_ofxPresetsManager
	presetsManager.refresh();
#endif

	//--

	// Session Settings

	params_Session.setName("AppSession");
	params_Session.add(indexFx);
	params_Session.add(bSolo);

#ifndef USE_ofxGui
	params_Session.add(bGui_Controls);
#endif

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	params_Session.add(guiManager.bMinimize);
#endif

#ifdef USE_ofxGui
	params_Session.add(position_Gui);
#endif

#ifdef USE_ofxSurfingPresets
	params_Session.add(presetsManager.bGui);
#endif

	//--

#ifdef USE_ofxPresetsManager

	setup_PresetsManager();

#endif

	//--

	// Callbacks

	ofAddListener(params_Control.parameterChangedE(), this, &ofxSurfingFxChannel::Changed);

}

#ifdef USE_ofxPresetsManager

//--------------------------------------------------------------
void ofxSurfingFxChannel::setup_PresetsManager()
{
	// customize
	presetsManager.setPath_UserKit_Folder(path_GLOBAL_Folder + "/ofxPresetsManager");
	//presetsManager.setPath_KitFolder("presets");
	//presetsManager.setPath_PresetsFolder("archive");
	//presetsManager.setPath_ControlSettings("settings");

	// user clicker
	presetsManager.setVisible_PresetClicker(true);
	//presetsManager.setPosition_PresetClicker(200, 100, 50);

	//-

	// add group params
	presetsManager.add(params_Preset, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
	presetsManager.setup("ofxSurfingFxChannel");//optional name

	//-

	params_ControlExternal.setName("CONTROLS");
	params_ControlExternal.add(bEnable_Fx);
	params_ControlExternal.add(presetsManager.getParamsControls());
}

#endif

//--------------------------------------------------------------
void ofxSurfingFxChannel::Changed(ofAbstractParameter& e)
{
	if (bDISABLE_CALLBACKS) return;

	std::string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << " : " << e;

	//--

	// Fx Selector

	if (name == indexFx.getName())
	{
		bDISABLE_CALLBACKS = true;

		// if solo mutes all and enables selected
		if (bSolo.get())
		{
			ENABLE_Monochrome.setWithoutEventNotifications(false);
			ENABLE_ThreeTones.setWithoutEventNotifications(false);
			ENABLE_HSB.setWithoutEventNotifications(false);

#ifdef USE_FX_DELAYS
			ENABLE_Delay.setWithoutEventNotifications(false);
			ENABLE_Echotrace.setWithoutEventNotifications(false);
#endif
			//--

			switch (indexFx.get())
			{
			case 1: ENABLE_Monochrome.setWithoutEventNotifications(true); break;
			case 2: ENABLE_ThreeTones.setWithoutEventNotifications(true); break;
			case 3: ENABLE_HSB.setWithoutEventNotifications(true); break;

#ifdef USE_FX_DELAYS
			case 4: ENABLE_Delay.setWithoutEventNotifications(true); break;
			case 5: ENABLE_Echotrace.setWithoutEventNotifications(true); break;
#endif
			}
		}

		//--

		//TODO:
		// workaround: to avoid feedback callbacks crashes..
		// set the name for selected.
		// set the name on next frame update!
		bFlagRefresh = true;

		//--

		if (bEnableGuiWorkflow) refresh_GuiWorkflow();

		bDISABLE_CALLBACKS = false;
	}

	//--

	// minimize

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	else if (name == guiManager.bMinimize.getName())
	{
		setupStyles();

		//if (bSolo) bSolo = false;
	}

#endif

	//--

	// solo

	else if (name == bSolo.getName())
	{
		bDISABLE_CALLBACKS = true;

		if (bSolo.get())
		{
			ENABLE_Monochrome_PRE = ENABLE_Monochrome;
			ENABLE_ThreeTones_PRE = ENABLE_ThreeTones;
			ENABLE_HSB_PRE = ENABLE_HSB;

#ifdef USE_FX_DELAYS
			ENABLE_Delay_PRE = ENABLE_Delay;
			ENABLE_Echotrace_PRE = ENABLE_Echotrace;
#endif
			bSoloed = true;

			//-

			ENABLE_Monochrome.setWithoutEventNotifications(false);
			ENABLE_ThreeTones.setWithoutEventNotifications(false);
			ENABLE_HSB.setWithoutEventNotifications(false);

#ifdef USE_FX_DELAYS
			ENABLE_Delay.setWithoutEventNotifications(false);
			ENABLE_Echotrace.setWithoutEventNotifications(false);
#endif
			//--

			switch (indexFx.get())
			{
			case 1: ENABLE_Monochrome.setWithoutEventNotifications(true); break;
			case 2: ENABLE_ThreeTones.setWithoutEventNotifications(true); break;
			case 3: ENABLE_HSB.setWithoutEventNotifications(true); break;

#ifdef USE_FX_DELAYS
			case 4: ENABLE_Delay.setWithoutEventNotifications(true); break;
			case 5: ENABLE_Echotrace.setWithoutEventNotifications(true); break;
#endif
			}
		}
		else //TODO: to improve workflow we should restore previous (PRE) states...
		{
			if (bSoloed)
			{
				bSoloed = false;
				{
					ENABLE_Monochrome.setWithoutEventNotifications(ENABLE_Monochrome_PRE);
					ENABLE_ThreeTones.setWithoutEventNotifications(ENABLE_ThreeTones_PRE);
					ENABLE_HSB.setWithoutEventNotifications(ENABLE_HSB_PRE);
#ifdef USE_FX_DELAYS
					ENABLE_Delay.setWithoutEventNotifications(ENABLE_Delay_PRE);
					ENABLE_Echotrace.setWithoutEventNotifications(ENABLE_Echotrace_PRE);
#endif
				}
			}
		}

		//--

		bDISABLE_CALLBACKS = false;

		//TODO:
		if (bEnableGuiWorkflow) refresh_GuiWorkflow();
	}

	//----

	// Enabler Toggles


	else if (name == ENABLE_Monochrome.getName())
	{
		if (ENABLE_Monochrome.get() && bEnableGuiWorkflow) {
			if (indexFx != 1) indexFx = 1;
		}
		else
		{
		}
	}
	else if (name == ENABLE_ThreeTones.getName() && bEnableGuiWorkflow)
	{
		if (ENABLE_ThreeTones.get()) {
			if (indexFx != 2) indexFx = 2;
		}
		else
		{
		}
	}
	else if (name == ENABLE_HSB.getName() && bEnableGuiWorkflow)
	{
		if (ENABLE_HSB.get()) {
			if (indexFx != 3) indexFx = 3;
		}
		else
		{
		}
	}

#ifdef USE_FX_DELAYS

	else if (name == ENABLE_Delay.getName() && bEnableGuiWorkflow)
	{
		if (ENABLE_Delay.get()) {
			if (indexFx != 4) indexFx = 4;
		}
		else
		{
		}
	}
	else if (name == ENABLE_Echotrace.getName() && bEnableGuiWorkflow)
	{
		if (ENABLE_Echotrace.get()) {
			if (indexFx != 5) indexFx = 5;
		}
		else
		{
		}
	}

#endif



	//----

	// all

	else if (name == bAll.getName())
	{
		if (bAll.get()) {
			bAll = false;

			if (bSolo.get()) bSolo = false;

			ENABLE_Monochrome = true;
			ENABLE_ThreeTones = true;
			ENABLE_HSB = true;

#ifdef USE_FX_DELAYS
			ENABLE_Delay = true;
			ENABLE_Echotrace = true;
#endif
		}
	}

	//-

	// none

	else if (name == bNone.getName())
	{
		if (bNone.get()) {
			bNone = false;

			if (bSolo.get()) bSolo = false;

			ENABLE_Monochrome = false;
			ENABLE_ThreeTones = false;
			ENABLE_HSB = false;

#ifdef USE_FX_DELAYS
			ENABLE_Delay = false;
			ENABLE_Echotrace = false;
#endif
		}
	}

	//-

	// reset

	else if (name == bReset.getName())
	{
		if (bReset.get()) {
			bReset = false;

			doReset();
		}
	}

	//--

	// presets

#ifdef USE_ofxPresetsManager

	else if (name == bGui_Presets.getName())
	{
		presetsManager.setVisible_PresetClicker(bGui_Presets);
	}

#endif

	//--

	// enable keys

	else if (name == bKeys.getName())
	{

#ifdef USE_ofxPresetsManager

		presetsManager.setEnableKeys(bKeys);

#endif
	}

	//--

	// position gui

#ifdef USE_ofxGui

	else if (name == position_Gui.getName())
	{
		gui.setPosition(position_Gui->x, position_Gui->y);
	}

#endif
}

//--

// keys
//--------------------------------------------------------------
void ofxSurfingFxChannel::keyPressed(ofKeyEventArgs& eventArgs)
{
	// disabler for all keys. (independent from bActive)
	if (!bKeys) return;

	const int& key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << (char)key << " [" << key << "]";

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	bool debug = false;
	if (debug)
	{
		ofLogNotice(__FUNCTION__) << "mod_COMMAND : " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL : " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT     : " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT   : " << (mod_SHIFT ? "ON" : "OFF");
	}

	//-

	if (bKeys)
	{
		// Browse Fx selector
		if (key == OF_KEY_UP)
		{
			if (indexFx.get() > 1) indexFx--;
		}

		else if (key == OF_KEY_DOWN)
		{
#ifndef USE_FX_DELAYS
			if (indexFx.get() < 3) indexFx++;
#else
			if (indexFx.get() < 5) indexFx++;
#endif
		}

		else if (key == OF_KEY_BACKSPACE) {
			doReset();
		}

		else if (key == 'S') {
			bSolo = !bSolo;
		}

		else if (key == 'A') {
			bAll = !bAll;
		}

		else if (key == 'N') {
			bNone = !bNone;
		}

		else if (key == 'E') {
			bEnable_Fx = !bEnable_Fx;
		}

		else if (key == 'G') {
			bGui = !bGui;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::begin() {
	//if (bEnable_Fx)
	{
		fbo_FxChain.begin();
		ofClear(0, 0, 0, 0);
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::end() {
	//if (bEnable_Fx)
	{
		fbo_FxChain.end();
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::update_FxChannel()
{
	//TODO:
	if (bFlagRefresh)
	{
		bFlagRefresh = false;

		refresh_FxName();
	}

	//--

	if (bEnable_Fx)
	{
		// fx
		frag1.apply(fbo_FxChain);
		frag3.apply(fbo_FxChain);
		frag2.apply(fbo_FxChain);

		// extra fx
#ifdef USE_FX_DELAYS
		frag4.apply(fbo_FxChain);
		frag5.apply(fbo_FxChain);
		//frag6.apply(fbo_FxChain);
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::windowResized(int _w, int _h)
{
	window_W = _w;
	window_H = _h;

	// resize fbo
	fboAllocate();
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawGui()
{
	if (bGui.get())
	{
#ifdef USE_ofxGui
		gui.draw();
#endif

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
		drawImGui();
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::draw()
{
	//if (bEnable_Fx)
	{
		update_FxChannel();

		ofSetColor(255, 255, 255, 255);

		if (!vflip)
		{
			fbo_FxChain.draw(0, 0, window_W, window_H);
		}
		else
		{
			fbo_FxChain.draw(0, window_H, window_W, -window_H);
		}
	}
}

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGui()
{
	guiManager.begin(); // global begin
	{
		drawImGuiMain();
		drawImGuiControls();
		drawImGuiUsers();
	}
	guiManager.end(); // global end

	//--

#ifdef USE_ofxSurfingPresets
	presetsManager.draw();
#endif

}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGuiMain()
{
	if (guiManager.beginWindow(bGui))
	{
		guiManager.Add(bEnable_Fx, OFX_IM_TOGGLE_BIG_BORDER);
		guiManager.AddSpacing();

		guiManager.Add(guiManager.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);

		guiManager.Add(bGui_Controls, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
		guiManager.Add(bGui_User, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

		if (!guiManager.bMinimize)
		{
			guiManager.Add(bKeys, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);

			////TODO:
			//// trig to implement workflow collapse / expand groups not working...
			//////guiManager.Add(bCollapse, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL, 2, true);
			//////guiManager.Add(bExpand, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL, 2, false);
			////TODO:
			//ImGui::Spacing();
			//guiManager.Add(bCollapse, OFX_IM_BUTTON_SMALL, 2, true);
			//guiManager.Add(bExpand, OFX_IM_BUTTON_SMALL, 2, false);
		}

		guiManager.AddSpacing();

		//--

#ifdef USE_ofxSurfingPresets
		if (!guiManager.bMinimize)
		{
			guiManager.AddSpacingSeparated();

			guiManager.Add(presetsManager.bGui_Global, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			if (presetsManager.bGui_Global)
			{
				guiManager.Indent();
				//guiManager.Add(presetsManager.bGui_Player, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
				presetsManager.draw_ImGui_ClickerSimple(false, false);
				guiManager.Unindent();
			}

			guiManager.AddSpacingSeparated();
		}
#endif
		//--

		ImGui::Spacing();

		//-

		// Controls

		if (!guiManager.bMinimize) guiManager.AddGroup(params_Preview, ImGuiTreeNodeFlags_None);

		//// Enable Fx Toggles
		//guiManager.AddGroup(params_Enablers, ImGuiTreeNodeFlags_None, OFX_IM_GROUP_COLLAPSED);

		//-

#ifdef USE_ofxPresetsManager
		if (!guiManager.bMinimize)
		{
			AddSpacingSeparated();
			guiManager.Add(bGui_Presets, OFX_IM_TOGGLE_BIG);
			ImGui::Spacing();
		}
#endif
		guiManager.endWindow();
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGuiUsers()
{
	if (bGui_Controls) {
		ImVec2 p;
		ImGuiContext* GImGui = ImGui::GetCurrentContext();
		ImGuiContext& g = *GImGui;
		for (ImGuiWindow* window : g.WindowsFocusOrder)
		{
			if (window->WasActive && window->Name == bGui_Controls.getName())
			{
				p = window->Pos + ImVec2(window->Size.x, 0);
				break;
			}
		}
		ImGuiCond cond = ImGuiCond_Always;
		ImGui::SetNextWindowPos(ImVec2(p.x, p.y), cond);
	}

	IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_MINI

		if (guiManager.beginWindow(bGui_User))
		{
			static SurfingImGuiTypes style = OFX_IM_HSLIDER_MINI;
			//static SurfingImGuiTypes style = OFX_IM_HSLIDER_SMALL;

			if (ENABLE_Monochrome) {
				guiManager.Add(frag1.low, style);
				guiManager.Add(frag1.high, style);
				//ofxImGuiSurfing::AddRangeParam("MONOCHROME", frag1.low, frag1.high);
			}
			if (ENABLE_ThreeTones) {
				if (ENABLE_Monochrome) guiManager.AddSeparator();
				guiManager.Add(frag2.mix, style);
				guiManager.Add(frag2.thresholds[0], style);
				guiManager.Add(frag2.thresholds[1], style);
				guiManager.Add(frag2.fade, style);
			}
			if (ENABLE_HSB) {
				if (ENABLE_Monochrome || ENABLE_ThreeTones) guiManager.AddSpacingSeparated();
				if (!ENABLE_Monochrome) guiManager.Add(frag3.hue, style);
				guiManager.Add(frag3.brightness, style);
				guiManager.Add(frag3.contrast, style);
			}

#ifdef USE_FX_DELAYS
			if (ENABLE_Delay) {
				if (ENABLE_Monochrome || ENABLE_ThreeTones || ENABLE_Delay)
					if (ENABLE_Echotrace) guiManager.AddSpacingSeparated();
				guiManager.Add(frag4.feedback, style);
			}
			if (ENABLE_Echotrace) {
				guiManager.AddSpacingSeparated();
				guiManager.Add(frag5.gain, style);
			}
#endif

			guiManager.endWindow();
		}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGuiControls()
{
	if (bGui)
	{
		ImVec2 p;
		ImGuiContext* GImGui = ImGui::GetCurrentContext();
		ImGuiContext& g = *GImGui;
		for (ImGuiWindow* window : g.WindowsFocusOrder)
		{
			if (window->WasActive && window->Name == bGui.getName())
			{
				p = window->Pos + ImVec2(window->Size.x, 0);
				break;
			}
		}
		ImGuiCond cond = ImGuiCond_Always;
		ImGui::SetNextWindowPos(ImVec2(p.x, p.y), cond);
	}

	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL

	if (guiManager.beginWindow(bGui_Controls))
	{
		guiManager.Add(ENABLE_Monochrome, OFX_IM_TOGGLE_BIG);
		if (!guiManager.bMinimize && frag1.active) guiManager.AddGroup(frag1.parameters, fgT1, fg, cond);

		guiManager.AddSpacingSeparated();

		guiManager.Add(ENABLE_ThreeTones, OFX_IM_TOGGLE_BIG);
		if (!guiManager.bMinimize && frag2.active) guiManager.AddGroup(frag2.parameters, fgT2, fg, cond);

		guiManager.AddSpacingSeparated();

		guiManager.Add(ENABLE_HSB, OFX_IM_TOGGLE_BIG);
		if (!guiManager.bMinimize && frag3.active) guiManager.AddGroup(frag3.parameters, fgT3, fg, cond);


#ifdef USE_FX_DELAYS	
		guiManager.AddSpacingSeparated();

		guiManager.Add(ENABLE_Delay, OFX_IM_TOGGLE_BIG);
		if (!guiManager.bMinimize && frag4.active) guiManager.AddGroup(frag4.parameters, fgT4, fg, cond);

		guiManager.AddSpacingSeparated();

		guiManager.Add(ENABLE_Echotrace, OFX_IM_TOGGLE_BIG);
		if (!guiManager.bMinimize && frag5.active) guiManager.AddGroup(frag5.parameters, fgT5, fg, cond);
#endif					

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
		if (cond == ImGuiCond_Always) cond = ImGuiCond_Appearing;
#endif					

		guiManager.endWindow();
	}
}

#endif

//--------------------------------------------------------------
void ofxSurfingFxChannel::exit()
{
	ofLogNotice(__FUNCTION__);

#ifdef USE_ofxPresetsManager
	presetsManager.exit();
#endif

	//--

	// position gui

#ifdef USE_ofxGui
	position_Gui = glm::vec2(gui.getPosition());
#endif

	//--

	// settings 

#ifndef USE_ofxPresetsManager
	ofxSurfingHelpers::saveGroup(params_Preset, path_GLOBAL_Folder + "/" + path_fileName_Preset);
#endif

	ofxSurfingHelpers::saveGroup(params_Session, path_GLOBAL_Folder + "/" + path_fileName_Session);
}

//--

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

//--------------------------------------------------------------
void ofxSurfingFxChannel::setupStyles() {

	guiManager.clearStyles(); // Clear queued styles if there where previously added, We can do this and changes styles on runtime!

	guiManager.AddStyle(indexFx, OFX_IM_HSLIDER_SMALL);
	//guiManager.AddStyle(indexFx, OFX_IM_HSLIDER_MINI);//TODO: fails

	if (guiManager.bMinimize)//fails
	{
		//guiManager.AddStyle(indexFx, OFX_IM_HIDDEN, 1, false);//fails
		//guiManager.AddStyle(indexFx_Name, OFX_IM_HIDDEN, 1 , false);
		guiManager.AddStyle(bSolo, OFX_IM_HIDDEN, 2, true);
		guiManager.AddStyle(bReset, OFX_IM_HIDDEN, 2, false);//spacing at end
		guiManager.AddStyle(bAll, OFX_IM_HIDDEN, 2, true);
		guiManager.AddStyle(bNone, OFX_IM_HIDDEN, 2, false, 5);//spacing at end
	}
	else
	{
		//guiManager.AddStyle(indexFx, OFX_IM_DEFAULT);
		//guiManager.AddStyle(indexFx_Name, OFX_IM_DEFAULT);
		guiManager.AddStyle(bSolo, OFX_IM_TOGGLE_SMALL_BORDER_BLINK, 2, true);
		guiManager.AddStyle(bReset, OFX_IM_TOGGLE_SMALL, 2, false);//spacing at end
		guiManager.AddStyle(bAll, OFX_IM_TOGGLE_SMALL, 2, true);
		guiManager.AddStyle(bNone, OFX_IM_TOGGLE_SMALL, 2, false, 5);//spacing at end
	}

	// hide alpha on colors
	for (size_t i = 0; i < frag2.colors.size(); i++)
	{
		guiManager.AddStyle(frag2.colors[i], OFX_IM_COLOR_NO_ALPHA);
	}


	SurfingImGuiTypes type = OFX_IM_HIDDEN;
	//SurfingImGuiTypes type = OFX_IM_TOGGLE_BIG;

	guiManager.AddStyle(ENABLE_Monochrome, type);
	guiManager.AddStyle(ENABLE_ThreeTones, type);
	guiManager.AddStyle(ENABLE_HSB, type);
#ifdef USE_FX_DELAYS
	guiManager.AddStyle(ENABLE_Delay, type);
	guiManager.AddStyle(ENABLE_Echotrace, type);
#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::ClearStyles() {
	guiManager.clearStyles(); // TODO: -> call from beginWindow/group
}

#endif

//--

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_FxName()
{
	switch (indexFx.get())
	{
	case 1: indexFx_Name = (ofToString("MONOCHROME")); break;
	case 2: indexFx_Name = (ofToString("THREETONES")); break;
	case 3: indexFx_Name = (ofToString("HSB")); break;
#ifdef USE_FX_DELAYS			  
	case 4: indexFx_Name = (ofToString("DELAY")); break;
	case 5: indexFx_Name = (ofToString("ECHOTRACE")); break;
#endif
	default: indexFx_Name = (ofToString(" ")); break;
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_GuiMinimize() {

#ifdef USE_ofxGui

	auto& g0 = gui.getGroup(params_Control.getName());

	auto& g1 = g0.getGroup(params_Preview.getName());
	auto& g2 = g0.getGroup(params_Enablers.getName());
	auto& g3 = g0.getGroup(params_Preset.getName());

	auto& gf1 = g3.getGroup(frag1.parameters.getName());
	auto& gf2 = g3.getGroup(frag2.parameters.getName());
	auto& gf3 = g3.getGroup(frag3.parameters.getName());
#ifdef USE_FX_DELAYS
	auto& gf4 = g3.getGroup(frag4.parameters.getName());
	auto& gf5 = g3.getGroup(frag5.parameters.getName());
#endif

	g1.minimize();
	g2.minimize();
	g3.minimize();

	gf1.minimize();
	gf2.minimize();
	gf3.minimize();
#ifdef USE_FX_DELAYS
	gf4.minimize();
	gf5.minimize();
#endif

	//--

	refresh_GuiWorkflow();

#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_GuiWorkflow() {

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	//TODO:
	// fold expand/collapse

	// all closed
	fgT1 = fgT2 = fgT3 = fgT4 = fgT5 = fgT6 = ImGuiTreeNodeFlags_None;

	// open selected
	switch (indexFx.get())
	{
	case 1: fgT1 = ImGuiTreeNodeFlags_DefaultOpen; break;
	case 2: fgT2 = ImGuiTreeNodeFlags_DefaultOpen; break;
	case 3: fgT3 = ImGuiTreeNodeFlags_DefaultOpen; break;

#ifdef USE_FX_DELAYS
	case 4: fgT4 = ImGuiTreeNodeFlags_DefaultOpen; break;
	case 54: fgT5 = ImGuiTreeNodeFlags_DefaultOpen; break;
#endif

	}

	cond = ImGuiCond_Always;

#endif

	//--

#ifdef USE_ofxGui

	auto& g0 = gui.getGroup(params_Control.getName());
	auto& g3 = g0.getGroup(params_Preset.getName());

	auto& gf1 = g3.getGroup(frag1.parameters.getName());
	auto& gf2 = g3.getGroup(frag2.parameters.getName());
	auto& gf3 = g3.getGroup(frag3.parameters.getName());
#ifdef USE_FX_DELAYS
	auto& gf4 = g3.getGroup(frag4.parameters.getName());
	auto& gf5 = g3.getGroup(frag5.parameters.getName());
#endif

	gf1.minimize();
	gf2.minimize();
	gf3.minimize();
#ifdef USE_FX_DELAYS
	gf4.minimize();
	gf5.minimize();
#endif

	if (bSolo)
	{
		if (ENABLE_Monochrome) gf1.maximize();
		if (ENABLE_ThreeTones) gf2.maximize();
		if (ENABLE_HSB) gf3.maximize();
#ifdef USE_FX_DELAYS
		if (ENABLE_Delay) gf4.maximize();
		if (ENABLE_Echotrace) gf5.maximize();
#endif
	}
	else
	{
		if (indexFx == 1) gf1.maximize();
		else if (indexFx == 2) gf2.maximize();
		else if (indexFx == 3) gf3.maximize();
#ifdef USE_FX_DELAYS
		else if (indexFx == 4) gf4.maximize();
		else if (indexFx == 5) gf5.maximize();
#endif
	}

#endif

}