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

		doReset1();
		doReset2();
		doReset3();

#ifdef USE_FX_DELAYS
		doReset4();
		doReset5();
#endif

		// active
		frag1.active = false;
		frag2.active = false;
		frag3.active = false;

#ifdef USE_FX_DELAYS
		frag4.active = false;
		frag5.active = false;
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::doReset1()
{
	//frag1.active = true;
	frag1.low = 0;
	frag1.high = 1;
}
//--------------------------------------------------------------
void ofxSurfingFxChannel::doReset2()
{
	//frag2.active = true;
	frag2.mix = 1;
	frag2.thresholds[0] = 0;
	frag2.thresholds[1] = 0.5f;
	frag2.fade = 0.3f;
	frag2.colors[0] = 0;
	frag2.colors[1] = 128;
	frag2.colors[2] = 255;
}
//--------------------------------------------------------------
void ofxSurfingFxChannel::doReset3()
{
	//frag3.active = true;
	frag3.hue = 0;
	frag3.saturation = 1;
	frag3.brightness = 0;
	frag3.contrast = 1;
}

#ifdef USE_FX_DELAYS
//--------------------------------------------------------------
void ofxSurfingFxChannel::doReset4()
{
	//frag4.active = false;
	frag4.feedback = 0;
	frag4.blendmode = 0;
}
//--------------------------------------------------------------
void ofxSurfingFxChannel::doReset5()
{
	//frag5.active = false;
	frag5.gain = 0.7f;
	frag5.threshold = 0.55f;
	frag5.invert = false;
	frag5.hardCutoff = false;
}
#endif

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
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__);

	//-

#ifdef USE_ofxGui

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();

	gui.setup("FX CH");
	gui.add(params_Control);

#endif

	//----

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	ui.setup();

	setupStyles();

#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::setup()
{
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__);

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
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__);
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::startup()
{
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__);

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
#ifdef USE_ofxGui
	refresh_GuiMinimize();
#endif

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
	params_Preview.add(bResetAll);
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
	frag4.active.setName("DELAY ");//fix weird problem on saving presets
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
	params_Control.add(ui.bMinimize);
#endif

#ifdef USE_ofxPresetsManager
	params_PresetsManagerTools.add(presetsManager.getParamsControls());
	// to use callbacks only
	params_Control.add(params_PresetsManagerTools);
	params_Control.add(bGui_Presets);
#endif

	//--

	// Resets

	listeners.push(bReset1.newListener([this] {
		doReset1();
		}));

	listeners.push(bReset2.newListener([this] {
		doReset2();
		}));

	listeners.push(bReset3.newListener([this] {
		doReset3();
		}));

#ifdef USE_FX_DELAYS
	listeners.push(bReset4.newListener([this] {
		doReset4();
		}));

	listeners.push(bReset5.newListener([this] {
		doReset5();
		}));
#endif

	//--

	// Exclude

	bResetAll.setSerializable(false);
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

	//TODO:
	// this should be auto handled by the ImGui manager as it is an special window
	params_Session.add(bGui);
	params_Session.add(bGui_User);

#ifndef USE_ofxGui
	params_Session.add(bGui_Controls);
#endif

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	params_Session.add(ui.bMinimize);
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

#ifdef USE_ofxSurfingPresetsLite
	presetsManager.setUiPtr(&ui);
	presetsManager.setPath(path_GLOBAL_Folder);

	presetsManager.AddGroup(params_Preset);
	presetsManager.setColorized(true);
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
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__) << name << " : " << e;

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

	else if (name == ui.bMinimize.getName())
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

	else if (name == bResetAll.getName())
	{
		if (bResetAll.get()) {
			bResetAll = false;

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
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__) << (char)key << " [" << key << "]";

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	bool debug = false;
	if (debug)
	{
		ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__) << "mod_COMMAND : " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__) << "mod_CONTROL : " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__) << "mod_ALT     : " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__) << "mod_SHIFT   : " << (mod_SHIFT ? "ON" : "OFF");
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
		// Fx
		frag1.apply(fbo_FxChain);
		frag3.apply(fbo_FxChain);
		frag2.apply(fbo_FxChain);

		// Extra Fx
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
	ui.Begin(); // global begin
	{
		drawImGuiMain();
		drawImGuiControls();
		drawImGuiUser();
	}
	ui.End(); // global end

	//--

#ifdef USE_ofxSurfingPresets
	presetsManager.draw();
#endif

}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGuiMain()
{
	if (ui.BeginWindow(bGui))
	{
		ui.Add(bEnable_Fx, OFX_IM_TOGGLE_BIG_BORDER);
		ui.AddSpacing();

		ui.Add(ui.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);

		ui.Add(bGui_Controls, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
		ui.Add(bGui_User, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

		//if (!ui.bMinimize)
		//{
		//	//ui.Add(bKeys, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);

		//	////TODO:
		//	//// trig to implement workflow collapse / expand groups not working...
		//	//////ui.Add(bCollapse, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL, 2, true);
		//	//////ui.Add(bExpand, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL, 2, false);
		//	////TODO:
		//	//ImGui::Spacing();
		//	//ui.Add(bCollapse, OFX_IM_BUTTON_SMALL, 2, true);
		//	//ui.Add(bExpand, OFX_IM_BUTTON_SMALL, 2, false);

		////ui.AddSpacingSeparated();
		//}

		//--

#ifdef USE_ofxSurfingPresets
		if (!ui.bMinimize)
		{
			ui.AddSpacingSeparated();

			ui.Add(presetsManager.bGui_Global, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			if (presetsManager.bGui_Global)
			{
				ui.Indent();
				//ui.Add(presetsManager.bGui_Player, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
				presetsManager.draw_ImGui_ClickerSimple(false, false);
				ui.Unindent();
			}

			ui.AddSpacingSeparated();
		}
#endif
		//--

		//ImGui::Spacing();

		//--

		// Controls

		if (!ui.bMinimize && bGui_Controls)
		{
			AddSpacingSeparated();
			ui.AddGroup(params_Preview, ImGuiTreeNodeFlags_None);
		}

		//// Enable Fx Toggles
		//ui.AddGroup(params_Enablers, ImGuiTreeNodeFlags_None, OFX_IM_GROUP_COLLAPSED);

		//--

#ifdef USE_ofxPresetsManager
		if (!ui.bMinimize)
		{
			AddSpacingSeparated();
			ui.Add(bGui_Presets, OFX_IM_TOGGLE_BIG);
			ImGui::Spacing();
		}
#endif
		//--

#ifdef USE_ofxSurfingPresetsLite
		ui.AddSpacingSeparated();

		//ui.Add(presetsManager.bGui, OFX_IM_TOGGLE_ROUNDED_SMALL);
		//if (presetsManager.bGui) 
		{
			presetsManager.drawImGui();
		}
#endif
		ui.EndWindow();
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGuiUser()
{
	if (!bGui_User) return;

	if (bGui_Controls) ui.setNextWindowAfterWindowNamed(bGui_Controls);
	else if (bGui) ui.setNextWindowAfterWindowNamed(bGui);

	IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;
	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_MINI;

	if (ui.BeginWindow(bGui_User))
	{
		static SurfingGuiTypes style = OFX_IM_HSLIDER_MINI;
		static SurfingGuiTypes style2 = OFX_IM_KNOB_TICKKNOB;
		//static SurfingGuiTypes style = OFX_IM_HSLIDER_SMALL;

		if (ENABLE_Monochrome)
		{
			ui.AddLabelBig("MONOCHROME", false, true);

			ui.Add(frag1.low, style2, 2, true);
			ui.Add(frag1.high, style2, 2);

			////TODO:
			////Range
			//string n = "MONOCHR";
			//float power = 1; 
			//float _sp = ui.getWidgetsSpacingX();
			//float _w = ui.getWidgetsWidth(1) /*- _sp*/;
			//float _h = ui.getWidgetsHeightUnit();
			//ImGui::PushItemWidth(_w);
			//std::string ss = "%.3f    %.3f";
			//ImVec2 shape = ImVec2(_w, _h);
			////ImVec2 shape = ImVec2(-1, -1);
			//bool bfullsize = true;
			//bool bNoLabel = true;
			//ofxImGuiSurfing::AddRangeParam(n, frag1.low, frag1.high, ss.c_str(), power, shape, bfullsize, bNoLabel);
			//ImGui::PopItemWidth();

			//ofxImGuiSurfing::AddRangeParam("MONOCHROME", frag1.low, frag1.high);
		}

		if (ENABLE_ThreeTones)
		{
			if (ENABLE_Monochrome) ui.AddSpacingSeparated();

			ui.AddLabelBig("THREETONES", false, true);

			if (!bGui_Controls) {
				for (size_t i = 0; i < frag2.colors.size(); i++) {
					ui.Add(frag2.colors[i], OFX_IM_COLOR_BOX_NO_NAME, frag2.colors.size());
					//ui.Add(frag2.colors[i], OFX_IM_COLOR_BOX_NO_NAME, frag2.colors.size());
					if (i < frag2.colors.size() - 1) ImGui::SameLine();
				}
			}

			ui.Add(frag2.thresholds[0], style2, 2, true);
			ui.Add(frag2.thresholds[1], style2, 2);

			ui.Add(frag2.mix, style2, 2, true);
			ui.Add(frag2.fade, style2, 2);
		}

		if (ENABLE_HSB)
		{
			if (ENABLE_Monochrome || ENABLE_ThreeTones) ui.AddSpacingSeparated();

			ui.AddLabelBig("HSB", false, true);

			ui.Add(frag3.brightness, style2, 2, true);
			ui.Add(frag3.contrast, style2, 2);
		}

#ifdef USE_FX_DELAYS
		if (ENABLE_Delay)
		{
			if (ENABLE_Monochrome || ENABLE_ThreeTones || ENABLE_HSB) ui.AddSpacingSeparated();
			ui.AddLabelBig("DELAY", false, true);

			if (!bGui_Controls) ui.AddCombo(frag4.blendmode, blendmode_names);
			//ui.Add(frag4.feedback, style);
			ui.Add(frag4.feedback, style2, 2);
		}

		if (ENABLE_Echotrace)
		{
			if (ENABLE_Monochrome || ENABLE_ThreeTones || ENABLE_HSB || ENABLE_Delay) ui.AddSpacingSeparated();
			ui.AddLabelBig("ECHOTRACE", false, true);
			//ui.Add(frag5.gain, style);
			ui.Add(frag5.gain, style2, 2);
		}
#endif
		//--

		if (!bGui_Controls)
		{
			ui.AddSpacingSeparated();

			ui.Add(bResetAll, OFX_IM_TOGGLE_SMALL);
		}
		ui.EndWindow();
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::draw_ImGui_GameMode()
{
	//if (ui.bGui_GameMode)
	{
		{

#ifdef USE_ofxSurfingPresetsLite
			ui.AddSpacingSeparated();

			//ui.Add(presetsManager.bGui, OFX_IM_TOGGLE_ROUNDED_SMALL);
			//if (presetsManager.bGui) 
			{
				presetsManager.drawImGui();
			}
#endif
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::drawImGuiControls()
{
	if (!bGui) return;
	if (!bGui_Controls) return;

	ui.setNextWindowAfterWindowNamed(bGui);

	IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;

	if (ui.BeginWindow(bGui_Controls))
	{
		ui.Add(ENABLE_Monochrome, OFX_IM_TOGGLE_BIG);
		if (ENABLE_Monochrome) {
			string n = "##";
			n += ENABLE_Monochrome.getName();
			if (ui.BeginTree(n, false, true, fgT1)) {
				if (!bGui_User) {
					ui.Add(frag1.low, OFX_IM_HSLIDER_MINI);
					ui.Add(frag1.high, OFX_IM_HSLIDER_MINI);
				}

				ui.AddSpacing();
				ui.Add(bReset1, OFX_IM_BUTTON_SMALL);
				ui.EndTree();
			}
		}
		ui.AddSpacingSeparated();

		ui.Add(ENABLE_ThreeTones, OFX_IM_TOGGLE_BIG);
		if (ENABLE_ThreeTones) {
			string n = "##";
			n += ENABLE_ThreeTones.getName();
			if (ui.BeginTree(n, false, true, fgT2)) {
				if (!bGui_User) {
					ui.Add(frag2.mix, OFX_IM_HSLIDER_MINI);
					ui.Add(frag2.fade, OFX_IM_HSLIDER_MINI);
				}

				for (size_t i = 0; i < frag2.colors.size(); i++) {
					//ui.Add(frag2.colors[i], OFX_IM_COLOR_BOX_NO_NAME, frag2.colors.size());
					//if (i < frag2.colors.size() - 1) ImGui::SameLine();
					ui.Add(frag2.colors[i], OFX_IM_COLOR_NO_INPUTS_NO_ALPHA);
				}

				if (!bGui_User) {
					ui.Add(frag2.thresholds[0], OFX_IM_HSLIDER_MINI);
					ui.Add(frag2.thresholds[1], OFX_IM_HSLIDER_MINI);
				}

				ui.AddSpacing();
				ui.Add(bReset2, OFX_IM_BUTTON_SMALL);
				ui.EndTree();
			}
		}
		ui.AddSpacingSeparated();

		ui.Add(ENABLE_HSB, OFX_IM_TOGGLE_BIG);
		if (ENABLE_HSB) {
			string n = "##";
			n += ENABLE_HSB.getName();
			if (ui.BeginTree(n, false, true, fgT3)) {
				ui.Add(frag3.hue, OFX_IM_HSLIDER_MINI);
				ui.Add(frag3.saturation, OFX_IM_HSLIDER_MINI);

				if (!bGui_User) {
					ui.Add(frag3.brightness, OFX_IM_HSLIDER_MINI);
					ui.Add(frag3.contrast, OFX_IM_HSLIDER_MINI);
				}

				ui.AddSpacing();
				ui.Add(bReset3, OFX_IM_BUTTON_SMALL);
				ui.EndTree();
			}
		}
		ui.AddSpacingSeparated();

#ifdef USE_FX_DELAYS	
		ui.Add(ENABLE_Delay, OFX_IM_TOGGLE_BIG);
		if (ENABLE_Delay) {
			string n = "##";
			n += ENABLE_Delay.getName();
			if (ui.BeginTree(n, false, true, fgT4)) {
				if (!bGui_User) {
					ui.Add(frag4.feedback, OFX_IM_HSLIDER_MINI);
				}

				ui.AddCombo(frag4.blendmode, blendmode_names);
				//ui.Add(frag4.blendmode, OFX_IM_HSLIDER_MINI);

				ui.AddSpacing();
				ui.Add(bReset4, OFX_IM_BUTTON_SMALL);
				ui.EndTree();
			}
		}
		ui.AddSpacingSeparated();

		ui.Add(ENABLE_Echotrace, OFX_IM_TOGGLE_BIG);
		if (ENABLE_Echotrace) {
			string n = "##";
			n += ENABLE_Echotrace.getName();
			if (ui.BeginTree(n, false, true, fgT5)) {
				if (!bGui_User) {
					ui.Add(frag5.gain, OFX_IM_HSLIDER_MINI);
				}

				ui.Add(frag5.threshold, OFX_IM_HSLIDER_MINI);
				ui.Add(frag5.invert, OFX_IM_TOGGLE_SMALL);
				ui.Add(frag5.hardCutoff, OFX_IM_TOGGLE_SMALL);

				ui.AddSpacing();
				ui.Add(bReset5, OFX_IM_BUTTON_SMALL);
				ui.EndTree();
			}
		}
		ui.AddSpacingSeparated();
#endif	

		ui.Add(bResetAll, OFX_IM_TOGGLE_SMALL);

		ui.EndWindow();
	}
}

#endif

//--------------------------------------------------------------
void ofxSurfingFxChannel::exit()
{
	ofLogNotice("ofxSurfingFxChannel") << (__FUNCTION__);

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

	ui.clearStyles(); // Clear queued styles if there where previously added, We can do this and changes styles on runtime!

	// Previews

	ui.AddStyle(indexFx, OFX_IM_HSLIDER_SMALL);
	//ui.AddStyle(indexFx, OFX_IM_HSLIDER_MINI);//TODO: fails

	if (ui.bMinimize)//fails
	{
		//ui.AddStyle(indexFx, OFX_IM_HIDDEN, 1, false);//fails
		//ui.AddStyle(indexFx_Name, OFX_IM_HIDDEN, 1 , false);
		ui.AddStyle(bSolo, OFX_IM_HIDDEN, 2, true);
		ui.AddStyle(bResetAll, OFX_IM_HIDDEN, 2, false);//spacing at end
		ui.AddStyle(bAll, OFX_IM_HIDDEN, 2, true);
		ui.AddStyle(bNone, OFX_IM_HIDDEN, 2, false, 5);//spacing at end
	}
	else
	{
		//ui.AddStyle(indexFx, OFX_IM_DEFAULT);
		//ui.AddStyle(indexFx_Name, OFX_IM_DEFAULT);
		ui.AddStyle(bSolo, OFX_IM_TOGGLE_SMALL_BORDER_BLINK, 2, true);
		ui.AddStyle(bResetAll, OFX_IM_TOGGLE_SMALL, 2, false);//spacing at end
		ui.AddStyle(bAll, OFX_IM_TOGGLE_SMALL, 2, true);
		ui.AddStyle(bNone, OFX_IM_TOGGLE_SMALL, 2, false, 5);//spacing at end
	}

	//	// hide alpha on colors
	//	for (size_t i = 0; i < frag2.colors.size(); i++)
	//	{
	//		ui.AddStyle(frag2.colors[i], OFX_IM_COLOR_NO_INPUTS_NO_ALPHA);
	//		//ui.AddStyle(frag2.colors[i], OFX_IM_COLOR_NO_ALPHA);
	//	}
	//
	//	SurfingGuiTypes type = OFX_IM_HIDDEN;
	//	//SurfingGuiTypes type = OFX_IM_TOGGLE_BIG;
	//
	//	ui.AddStyle(ENABLE_Monochrome, type);
	//	ui.AddStyle(ENABLE_ThreeTones, type);
	//	ui.AddStyle(ENABLE_HSB, type);
	//
	//#ifdef USE_FX_DELAYS
	//	ui.AddStyle(ENABLE_Delay, type);
	//	ui.AddStyle(ENABLE_Echotrace, type);
	//#endif
	//
	//	//all resets called "reset" share the same name / style
	//	ui.AddStyle(bReset1, OFX_IM_TOGGLE_SMALL);

	ui.AddStyle(bEnableGuiWorkflow, OFX_IM_HIDDEN);
	ui.AddStyle(bKeys, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::ClearStyles() {
	ui.clearStyles(); // TODO: -> call from beginWindow/group
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

#ifdef USE_ofxGui
//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_GuiMinimize() {

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
	}
#endif

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_GuiWorkflow() {

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	//TODO:
	// fold expand/collapse

	// all closed
#ifdef USE_FX_DELAYS	
	fgT1 = fgT2 = fgT3 = fgT4 = fgT5 = fgT6 = ImGuiTreeNodeFlags_None;
#else
	fgT1 = fgT2 = fgT3 = fgT4 = ImGuiTreeNodeFlags_None;
#endif

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

	//cond = ImGuiCond_Always;

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