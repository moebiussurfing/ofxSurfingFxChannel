#include "ofxSurfingFxChannel.h"

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

		// clear
		fbo_FxChain.begin();
		ofClear(0, 0, 0, 255);
		fbo_FxChain.end();

		//-

		// fx
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
void ofxSurfingFxChannel::setup()
{
	bDISABLECALLBACKS = true;

	fboAllocate();

	//-

	setup_FxChannel();

	//----

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL

	guiManager.setup();

	//TODO:
	// Gui Workflow
	fg0 = ImGuiTreeNodeFlags_DefaultOpen;
	//fg0 = ImGuiTreeNodeFlags_CollapsingHeader;
	//fg0 = ImGuiTreeNodeFlags_None;

	fg1 = fg0;
	fg2 = fg0;
	fg3 = fg0;
	fg4 = fg0;
	fg5 = fg0;
	fg6 = fg0;

	setupStyles();

#endif

	//----

	// Startup 
	startup();
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::startup()
{
	ofLogNotice(__FUNCTION__);

	//-

	bDISABLECALLBACKS = false;

	//-

	// Load settings

	ofxSurfingHelpers::loadGroup(params_Session, path_GLOBAL_Folder + "/" + path_fileName_Session);

	// Not required when handled by the presets manager
#ifndef USE_ofxPresetsManager
	ofxSurfingHelpers::loadGroup(params_Preset, path_GLOBAL_Folder + "/" + path_fileName_Preset);
#endif

	//-

	// Presets Manager

	// handles fx presets! (with all the settings)

#ifdef USE_ofxSurfingPresets

	presetsManager.setPathPresets(path_GLOBAL_Folder + "/" + "Presets");

	// Reduce default amount of presets
	presetsManager.setMaxPresetsAmount(4);

	// add params
	presetsManager.addGroup(params_Preset);

	// some preferences for this situation: being minimal
	presetsManager.bGui.setName("Fx Presets");
	presetsManager.bGui = true;
	presetsManager.bGui_Editor = false;
	presetsManager.bGui_Parameters = false;
	presetsManager.bGui_FloatingClicker = false;
	presetsManager.bGui_InnerClicker = true;
	presetsManager.bMinimize_Clicker = true;
	
	presetsManager.bKeys.makeReferenceTo(bKeys);//link toggles
#endif

#ifdef USE_ofxPresetsManager
	presetsManager.refresh();
#endif

	//-

	// Refresh Gui workflow

	//TODO:
	bEnableGuiWorkflow = true;
	//	if (bEnableGuiWorkflow)
	//	{
	//		refresh_Gui();
	//	}
	//	else
	//	{
	//	}
	//
	//#ifdef USE_ofxGui
	//	refresh_Gui_minimize();
	//#endif
	//
	//#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	//	refresh_Gui_minimize();
	//#endif

	//-

	// Reinitialize or overwrite some possibly loaded settings.

	//guiManager.bMinimize = false;

	//setGuiPosition(glm::vec2(ofGetWidth() - 230, 10));
	//setEnableKeys(true);
}

////--------------------------------------------------------------
//void ofxSurfingFxChannel::setActive(bool b)
//{
//	bActive = b;
//	setGuiVisible(b);
//	setKeysEnable(b);
//}

////--------------------------------------------------------------
//void ofxSurfingFxChannel::setKeysEnable(bool b)
//{
//	bKeys = b;
//
//	presetsManager.setEnableKeys(b);
//}

//--------------------------------------------------------------
void ofxSurfingFxChannel::setup_FxChannel()
{
	// parameters

	// customize names to tweak gui labels
	frag1.parameters.setName("1 " + frag1.parameters.getName());
	frag2.parameters.setName("2 " + frag2.parameters.getName());
	frag3.parameters.setName("3 " + frag3.parameters.getName());
#ifdef USE_FX_DELAYS
	frag4.parameters.setName("4 " + frag4.parameters.getName());
	frag5.parameters.setName("5 " + frag5.parameters.getName());
#endif

	//--

	// gui layout
#ifdef USE_ofxGui
	position_Gui.set("GUI POSITION",
		glm::vec2(window_W * 0.5, window_H * 0.5),
		glm::vec2(0, 0),
		glm::vec2(window_W, window_H)
	);
#endif

	//--

	// Presets Params
	params_Preset.setName("FX_CHANNEL");

	// fx
	params_Preset.add(frag1.parameters);
	params_Preset.add(frag2.parameters);
	params_Preset.add(frag3.parameters);

	// extra fx
#ifdef USE_FX_DELAYS
	params_Preset.add(frag4.parameters);
	params_Preset.add(frag5.parameters);
	//params_Preset.add(frag6.parameters);
	SELECT_Fx.setMax(5);
#endif

	//--

	// exclude
	SELECT_Fx_Name.setSerializable(false);
	bSolo.setSerializable(false);
	bReset.setSerializable(false);
	bAll.setSerializable(false);
	bNone.setSerializable(false);

	//--

	params_Session.setName("AppSession");
	params_Session.add(SELECT_Fx);
	params_Session.add(bSolo);
	params_Session.add(guiManager.bMinimize);
	params_Session.add(bSettings);

#ifdef USE_ofxGui
	params_Session.add(position_Gui);
#endif
#ifdef USE_ofxSurfingPresets
	params_Session.add(presetsManager.bGui);
#endif

	//--

	// Gui

	// 1. user main controls

	params_Subcontrol.setName("CONTROL");
	params_Subcontrol.add(SELECT_Fx_Name);
	params_Subcontrol.add(SELECT_Fx);
	params_Subcontrol.add(bSolo);
	params_Subcontrol.add(bReset);
	params_Subcontrol.add(bAll);
	params_Subcontrol.add(bNone);

	//-

	// 2. edit fx params

	// linked enablers
	ENABLE_Monochrome.makeReferenceTo(frag1.active);
	ENABLE_ThreeTones.makeReferenceTo(frag2.active);
	ENABLE_HSB.makeReferenceTo(frag3.active);
#ifdef USE_FX_DELAYS
	ENABLE_Delay.makeReferenceTo(frag4.active);
	ENABLE_Echotrace.makeReferenceTo(frag5.active);
#endif

	//	// renames
	//	ENABLE_Monochrome.setName("ENABLE MONOCHROME");
	//	ENABLE_ThreeTones.setName("ENABLE THREETONES");
	//	ENABLE_HSB.setName("ENABLE HSB");
	//#ifdef USE_FX_DELAYS
	//	ENABLE_Delay.setName("ENABLE DELAY");
	//	ENABLE_Echotrace.setName("ENABLE ECHOTRACE");
	//#endif

	// renames
	ENABLE_Monochrome.setName("MONOCHROME");
	ENABLE_ThreeTones.setName("THREETONES");
	ENABLE_HSB.setName("HSB");
#ifdef USE_FX_DELAYS
	ENABLE_Delay.setName("DELAY");
	ENABLE_Echotrace.setName("ECHOTRACE");
#endif

	// queue params
	params_Subcontrol2.setName("SETTINGS");
	params_Subcontrol2.add(ENABLE_Monochrome);
	params_Subcontrol2.add(ENABLE_ThreeTones);
	params_Subcontrol2.add(ENABLE_HSB);
#ifdef USE_FX_DELAYS
	params_Subcontrol2.add(ENABLE_Delay);
	params_Subcontrol2.add(ENABLE_Echotrace);
#endif

	//--

	// to use callbacks only

	params_Control.setName("ofxSurfingFxChannel");
	params_Control.add(params_Subcontrol);
	params_Control.add(params_Subcontrol2);
	params_Control.add(params_Preset);
	params_Control.add(bCollapse);
	params_Control.add(bExpand);
	params_Control.add(guiManager.bMinimize);
	params_Control.add(bKeys);

#ifdef USE_ofxPresetsManager
	params_PresetsManagerTools.add(presetsManager.getParamsControls());
	// to use callbacks only
	params_Control.add(params_PresetsManagerTools);
	params_Control.add(bGui_Presets);
#endif

	//--

	// exclude
	bCollapse.setSerializable(false);
	bExpand.setSerializable(false);

	//-

#ifdef USE_ofxGui
	parameters.setName("ofxSurfingFxChannel");
	parameters.add(params_Control);
	//parameters.add(params_ControlExternal);
	//parameters.add(params_Preset);

	//-

	ofxSurfingHelpers::setThemeDark_ofxGui();
	gui.setup();
	gui.add(parameters);
	gui.setPosition(10, 20);
#endif

	//-

	// callbacks
	ofAddListener(params_Control.parameterChangedE(), this, &ofxSurfingFxChannel::Changed_params_Control);

	//-

#ifdef USE_ofxPresetsManager
	setup_PresetsManager();
#endif
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
	params_ControlExternal.add(bENABLE_Fx);
	params_ControlExternal.add(presetsManager.getParamsControls());
}
#endif

//--------------------------------------------------------------
void ofxSurfingFxChannel::Changed_params_Control(ofAbstractParameter &e)
{
	if (!bDISABLECALLBACKS)
	{
		std::string name = e.getName();
		//if (name != "" && name != "exclude")
		{
			ofLogNotice(__FUNCTION__) << name << " : " << e;
		}

		if (false) {}

		//--

		// Fx Selector

		else if (name == SELECT_Fx.getName())
		{
			// if solo mutes all and enables selected
			if (bSolo.get())
			{
				frag1.active = false;
				frag2.active = false;
				frag3.active = false;

#ifdef USE_FX_DELAYS
				frag4.active = false;
				frag5.active = false;
#endif
				switch (SELECT_Fx.get())
				{
				case 1:
					frag1.active = true;
					break;

				case 2:
					frag2.active = true;
					break;

				case 3:
					frag3.active = true;
					break;

#ifdef USE_FX_DELAYS
				case 4:
					frag4.active = true;
					break;

				case 5:
					frag5.active = true;
					break;
#endif
				}
			}

			// set the name for selected
			switch (SELECT_Fx.get())
			{
			case 0:
				SELECT_Fx_Name = "";
				break;

			case 1:
				SELECT_Fx_Name = "MONOCHROME";
				break;

			case 2:
				SELECT_Fx_Name = "THREETONES";
				break;

			case 3:
				SELECT_Fx_Name = "HSB";
				break;

#ifdef USE_FX_DELAYS
			case 4:
				SELECT_Fx_Name = "DELAY";
				break;

			case 5:
				SELECT_Fx_Name = "ECHOTRACE";
				break;
#endif
			}

			//--

			//TODO:
			//if (bEnableGuiWorkflow) refresh_Gui();

			//--
		}

		//--

		// minimize

		else if (name == guiManager.bMinimize.getName())
		{
			setupStyles();

			if (bSolo) bSolo = false;
		}

		// solo

		else if (name == bSolo.getName())
		{
			if (bSolo.get())
			{
				frag1.active = false;
				frag2.active = false;
				frag3.active = false;

#ifdef USE_FX_DELAYS
				frag4.active = false;
				frag5.active = false;
#endif
				switch (SELECT_Fx.get())
				{
				case 1:
					frag1.active = true;
					break;

				case 2:
					frag2.active = true;
					break;

				case 3:
					frag3.active = true;
					break;

#ifdef USE_FX_DELAYS
				case 4:
					frag4.active = true;
					break;

				case 5:
					frag5.active = true;
					break;
#endif
				}
			}
			else //TODO: to improve workflow we should restore previous (PRE) states...
			{
				switch (SELECT_Fx.get())
				{
				case 1:
				case 2:
				case 3:

#ifdef USE_FX_DELAYS
				case 4:
				case 5:
#endif
					frag1.active = true;
					frag2.active = true;
					frag3.active = true;

#ifdef USE_FX_DELAYS
					frag4.active = true;
					frag5.active = true;
#endif
					break;
				}
			}
		}

		//----

		// enabler toggles

		else if (name == ENABLE_Monochrome.getName())
		{
			if (ENABLE_Monochrome.get() && bEnableGuiWorkflow) {
				if (SELECT_Fx.get() != 1) {
					SELECT_Fx = 1;
				}
				//refresh_ofxGuiExtended_Check();
			}
			else
			{
				//refresh_Gui();
				//refresh_Gui_Solos();
			}
		}
		else if (name == ENABLE_ThreeTones.getName() && bEnableGuiWorkflow)
		{
			if (ENABLE_ThreeTones.get()) {
				if (SELECT_Fx.get() != 2) {
					SELECT_Fx = 2;
				}
				//refresh_ofxGuiExtended_Check();
			}
			else
			{
				//refresh_Gui();
				//refresh_Gui_Solos();
			}
		}
		else if (name == ENABLE_HSB.getName() && bEnableGuiWorkflow)
		{
			if (ENABLE_HSB.get()) {
				if (SELECT_Fx.get() != 3) {
					SELECT_Fx = 3;
				}
				//refresh_ofxGuiExtended_Check();
			}
			else
			{
				//refresh_Gui();
				//refresh_Gui_Solos();
			}
		}

#ifdef USE_FX_DELAYS
		else if (name == ENABLE_Delay.getName() && bEnableGuiWorkflow)
		{
			if (ENABLE_Delay.get()) {
				if (SELECT_Fx.get() != 4) {
					SELECT_Fx = 4;
				}
				//refresh_ofxGuiExtended_Check();
			}
			else
			{
				//refresh_Gui();
				//refresh_Gui_Solos();
			}
		}
		else if (name == ENABLE_Echotrace.getName() && bEnableGuiWorkflow)
		{
			if (ENABLE_Echotrace.get()) {
				if (SELECT_Fx.get() != 5) {
					SELECT_Fx = 5;
				}
				//refresh_ofxGuiExtended_Check();
			}
			else
			{
				//refresh_Gui();
				//refresh_Gui_Solos();
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

				frag1.active = true;
				frag2.active = true;
				frag3.active = true;
#ifdef USE_FX_DELAYS
				frag4.active = true;
				frag5.active = true;
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

				frag1.active = false;
				frag2.active = false;
				frag3.active = false;
#ifdef USE_FX_DELAYS
				frag4.active = false;
				frag5.active = false;
#endif
			}
		}

		//-

		// reset

		else if (name == bReset.getName())
		{
			if (bReset.get()) {
				bReset = false;

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

				//-

				//TODO:
				//reset only the selected fx
//				switch (SELECT_Fx.get())
//				{
//				case 1:
//					frag1.active = true;
//					frag1.low = 0;
//					frag1.high = 1;
//					break;
//
//				case 2:
//					frag2.active = true;
//					frag2.mix = 1;
//					frag2.thresholds[0] = 0;
//					frag2.thresholds[1] = 0.5f;
//					frag2.fade = 0.3f;
//					frag2.colors[0] = 0;
//					frag2.colors[1] = 128;
//					frag2.colors[2] = 255;
//					break;
//
//				case 3:
//					frag3.active = true;
//					frag3.hue = 0;
//					frag3.brightness = 0;
//					frag3.contrast = 1;
//					frag3.saturation = 1;
//					break;
//
//#ifdef USE_FX_DELAYS
//				case 4:
//					frag4.active = true;
//					frag4.feedback = 0;
//					frag4.blendmode = 0;
//					break;
//
//				case 5:
//					frag5.active = true;
//					frag5.gain = 0.7f;
//					frag5.threshold = 0.55f;
//					frag5.invert = false;
//					frag5.hardCutoff = false;
//					break;
//#endif
//				}
			}
		}

		//----

		//TODO:
		// gui workflow

		// expand

		else if (name == bExpand.getName())
		{
			if (bExpand.get())
			{
				bExpand = false;

				bOpenFrag1 = true;
				bOpenFrag2 = true;
				bOpenFrag3 = true;
#ifdef USE_FX_DELAYS
				bOpenFrag4 = true;
				bOpenFrag5 = true;
				//bOpenFrag6 = true;
#endif

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
				fg0 = ImGuiTreeNodeFlags_DefaultOpen;
				fg1 = fg0;
				fg2 = fg0;
				fg3 = fg0;
#ifdef USE_FX_DELAYS
				fg4 = fg0;
				fg5 = fg0;
				//fg6 = fg0;
#endif
#endif
			}
		}

		//--

		// collapse

		else if (name == bCollapse.getName())
		{
			if (bCollapse.get())
			{
				bCollapse = false;

				bOpenFrag1 = false;
				bOpenFrag2 = false;
				bOpenFrag3 = false;
#ifdef USE_FX_DELAYS
				bOpenFrag4 = false;
				bOpenFrag5 = false;
				//bOpenFrag6 = false;
#endif

#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
				fg0 = ImGuiTreeNodeFlags_CollapsingHeader;
				fg1 = fg0;
				fg2 = fg0;
				fg3 = fg0;
#ifdef USE_FX_DELAYS
				fg4 = fg0;
				fg5 = fg0;
				//fg6 = fg0;
#endif
#endif

				//refresh_Gui_minimize();

				//--

#ifdef USE_ofxGui
				// minimize
				auto &g0 = gui.getGroup(params_Control.getName());
				auto &g1 = g0.getGroup(params_Control.getName());
				auto &gus = g1.getGroup(params_Session.getName());
				auto &gfx = g1.getGroup(params_Preset.getName());
				gus.minimize();
				gfx.minimize();
				gui.minimizeAll();
#endif
			}
		}

		//--

		// show presets

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
	}
}

// keys
//--------------------------------------------------------------
void ofxSurfingFxChannel::keyPressed(ofKeyEventArgs &eventArgs)
{
	// disabler for all keys. (independent from bActive)
	if (!bKeys) return;

	const int &key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << (char)key << " [" << key << "]";

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	bool debug = false;
	if (debug)
	{
		ofLogNotice(__FUNCTION__) << "mod_COMMAND: " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL: " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT: " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT: " << (mod_SHIFT ? "ON" : "OFF");
	}

	//-

	if (bKeys)
	{
		// Browse Fx selector
		if (key == OF_KEY_UP)
		{
			if (SELECT_Fx.get() > 0) SELECT_Fx--;
		}

		else if (key == OF_KEY_DOWN) 
		{
#ifndef USE_FX_DELAYS
			if (SELECT_Fx.get() < 3) SELECT_Fx++;
#else
			if (SELECT_Fx.get() < 5) SELECT_Fx++;
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
			bENABLE_Fx = !bENABLE_Fx;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::begin() {
	//if (bENABLE_Fx)
	{
		fbo_FxChain.begin();
		ofClear(0, 0, 0, 0);
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::end() {
	//if (bENABLE_Fx)
	{
		fbo_FxChain.end();
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::update_FxChannel()
{
	if (bENABLE_Fx)
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
	//if (bENABLE_Fx)
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
		static bool bOpen1 = true;
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			if (guiManager.bAutoResize) window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

			guiManager.beginWindow("FX CHANNEL", &bOpen1, window_flags);
			{
				// Global Enable
				guiManager.Add(bENABLE_Fx, OFX_IM_TOGGLE_BIG_XXL_BORDER);
				ImGui::Spacing();

				guiManager.Add(bSettings, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

				guiManager.Add(guiManager.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);

				if (!guiManager.bMinimize)
				{
					guiManager.Add(bKeys, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);

					////guiManager.Add(bCollapse, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL, 2, true);
					////guiManager.Add(bExpand, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL, 2, false);
					//
					////TODO:
					//ImGui::Spacing();
					//guiManager.Add(bCollapse, OFX_IM_BUTTON_SMALL, 2, true);
					//guiManager.Add(bExpand, OFX_IM_BUTTON_SMALL, 2, false);
				}

				ImGui::Spacing();

				//--

#ifdef USE_ofxSurfingPresets
				ImGui::Spacing();

				presetsManager.draw_ImGui_MiniClicker();

				guiManager.refreshLayout();
				//ofxImGuiSurfing::AddSpacingSeparated();
#endif
				//--

				ImGui::Spacing();

				//-

				// Controls

				if (!guiManager.bMinimize) guiManager.AddGroup(params_Subcontrol, ImGuiTreeNodeFlags_None);

				guiManager.AddGroup(params_Subcontrol2);

#ifdef USE_ofxPresetsManager
				if (!guiManager.bMinimize)
				{
					AddSpacingSeparated();
					guiManager.Add(bGui_Presets, OFX_IM_TOGGLE_BIG);
					ImGui::Spacing();
				}
#endif
			}
			guiManager.endWindow();
		}

		//--

		// Fx groups
		if (frag1.active ||
			frag2.active ||
			frag3.active
#ifdef USE_FX_DELAYS
			||
			frag4.active ||
			frag5.active
#endif
			)
			if (bSettings)
			{
				{
					ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
					if (guiManager.bAutoResize) window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

					guiManager.beginWindow("FX SETTINGS", &(bool)bSettings.get(), window_flags);
					{
						if (frag1.active) guiManager.AddGroup(frag1.parameters, fg1, OFX_IM_GROUP_DEFAULT);
						if (frag2.active) guiManager.AddGroup(frag2.parameters, fg2, OFX_IM_GROUP_DEFAULT);
						if (frag3.active) guiManager.AddGroup(frag3.parameters, fg3, OFX_IM_GROUP_DEFAULT);
#ifdef USE_FX_DELAYS
						if (frag4.active) guiManager.AddGroup(frag4.parameters, fg4, OFX_IM_GROUP_DEFAULT);
						if (frag5.active) guiManager.AddGroup(frag5.parameters, fg5, OFX_IM_GROUP_DEFAULT);
#endif
					}
					guiManager.endWindow();
				}
			}
	}
	guiManager.end(); // global end

	//--

#ifdef USE_ofxSurfingPresets
	presetsManager.draw();
#endif

}
#endif

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_ofxGuiExtended_Check()
{
	// for gui extended only!
	//	ofLogNotice(__FUNCTION__);
	//
	//	//--
	//
	//#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	//		// all fx disabled
	//	if ((!ENABLE_Monochrome.get()) && (!ENABLE_HSB.get()) && (!ENABLE_ThreeTones.get())
	//#ifdef USE_FX_DELAYS
	//		&& (!ENABLE_Delay.get()) && (!ENABLE_Echotrace.get())
	//#endif
	//		)
	//	{
	//		bOpenFrag1 = false;
	//		bOpenFrag2 = false;
	//		bOpenFrag3 = false;
	//#ifdef USE_FX_DELAYS
	//		bOpenFrag4 = false;
	//		bOpenFrag5 = false;
	//		//bOpenFrag6 = false;
	//#endif
	//	}
	//
	//	// if some fx enabled and all are minimized
	//	else if
	//		(
	//			!bOpenFrag1 &&
	//			!bOpenFrag2 &&
	//			!bOpenFrag3
	//#ifdef USE_FX_DELAYS
	//			&& !bOpenFrag4 &&
	//			!bOpenFrag5
	//#endif
	//			)
	//	{
	//		switch (SELECT_Fx.get())
	//		{
	//		case 1:	if (ENABLE_Monochrome.get()) bOpenFrag1 = true; break;
	//		case 2:	if (ENABLE_ThreeTones.get()) bOpenFrag2 = true; break;
	//		case 3:	if (ENABLE_HSB.get()) bOpenFrag3 = true; break;
	//#ifdef USE_FX_DELAYS
	//		case 4:	if (ENABLE_Delay.get()) bOpenFrag4 = true; break;
	//		case 5:	if (ENABLE_Echotrace.get()) bOpenFrag5 = true; break;
	//#endif
	//		}
	//	}
	//
	//#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_Gui_minimize(bool bUseSolo)// for ofxGui mode only!
{
	//#ifdef USE_IM_GUI__OFX_SURFING_FX_CHANNEL
	//	if (bUseSolo)
	//	{
	//		bOpenFrag1 = false;
	//		bOpenFrag2 = false;
	//		bOpenFrag3 = false;
	//#ifdef USE_FX_DELAYS
	//		bOpenFrag4 = false;
	//		bOpenFrag5 = false;
	//		//bOpenFrag6 = false;
	//#endif
	//
	//		// workflow
	//		// expand selected fx panel
	//		//if (bSolo.get())
	//		if (bUseSolo)
	//		{
	//			switch (SELECT_Fx.get())
	//			{
	//			case 1:	if (ENABLE_Monochrome.get()) bOpenFrag1 = true; break;
	//			case 2:	if (ENABLE_ThreeTones.get()) bOpenFrag2 = true; break;
	//			case 3:	if (ENABLE_HSB.get()) bOpenFrag3 = true; break;
	//#ifdef USE_FX_DELAYS
	//			case 4:	if (ENABLE_Delay.get()) bOpenFrag4 = true; break;
	//			case 5:	if (ENABLE_Echotrace.get()) bOpenFrag5 = true; break;
	//#endif
	//			}
	//		}
	//
	//		//fg0 = ImGuiTreeNodeFlags_None;
	//		fg0 = ImGuiTreeNodeFlags_CollapsingHeader;
	//		fg1 = fg0;
	//		fg2 = fg0;
	//		fg3 = fg0;
	//		fg4 = fg0;
	//		fg5 = fg0;
	//		fg6 = fg0;
	//
	//		if (bOpenFrag1) fg1 |= ImGuiTreeNodeFlags_DefaultOpen;
	//		if (bOpenFrag2) fg2 |= ImGuiTreeNodeFlags_DefaultOpen;
	//		if (bOpenFrag3) fg3 |= ImGuiTreeNodeFlags_DefaultOpen;
	//#ifdef USE_FX_DELAYS
	//		if (bOpenFrag4) fg4 |= ImGuiTreeNodeFlags_DefaultOpen;
	//		if (bOpenFrag5) fg5 |= ImGuiTreeNodeFlags_DefaultOpen;
	//		//if (bOpenFrag6) fg6 |= ImGuiTreeNodeFlags_DefaultOpen;
	//#endif
	//	}
	//#endif
	//
	//	//--
	//
	//#ifdef USE_ofxGui
	//	auto &g0 = gui.getGroup(params_Control.getName());
	//	auto &g1 = g0.getGroup(params_Control.getName());
	//	auto &gus = g1.getGroup(params_Session.getName());
	//	auto &gfx = g1.getGroup(params_Preset.getName());
	//
	//	//gus.minimize();
	//	//gfx.minimize();
	//
	//	// frags
	//	auto &gd1 = gfx.getGroup(frag1.parameters.getName());
	//	auto &gd2 = gfx.getGroup(frag2.parameters.getName());
	//	auto &gd3 = gfx.getGroup(frag3.parameters.getName());
	//	gd1.minimize();
	//	gd2.minimize();
	//	gd3.minimize();
	//
	//#ifdef USE_FX_DELAYS	
	//	auto &gd4 = gfx.getGroup(frag4.parameters.getName());
	//	auto &gd5 = gfx.getGroup(frag5.parameters.getName());
	//	//auto &gd6 = gfx.getGroup(frag6.parameters.getName());
	//	gd4.minimize();
	//	gd5.minimize();
	//	//gd6.minimize();
	//#endif
	//
	//	if (bUseSolo)
	//	{
	//		// workflow
	//		// expand selected fx panel
	//		//if (bSolo.get())
	//		{
	//			switch (SELECT_Fx.get())
	//			{
	//			case 1:	if (ENABLE_Monochrome.get()) gd1.maximize(); break;
	//			case 2:	if (ENABLE_ThreeTones.get()) gd2.maximize(); break;
	//			case 3:	if (ENABLE_HSB.get()) gd3.maximize(); break;
	//#ifdef USE_FX_DELAYS
	//			case 4:	if (ENABLE_Delay.get()) gd4.maximize(); break;
	//			case 5:	if (ENABLE_Echotrace.get()) gd5.maximize(); break;
	//#endif
	//			}
	//		}
	//	}
	//
	//#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_Gui()// extended or simple gui
{
	//	// TODO: maybe there's some refresh method duplicated..
	//
	//	// handles solo's worklow
	//
	//	ofLogNotice(__FUNCTION__);
	//
	//	//-
	//
	//#ifdef USE_ofxGui
	//	refresh_Gui_minimize(true);
	//#endif
	//
	//	//-
	//
	//	//if (bSolo.get())
	//	{
	//		switch (SELECT_Fx.get())
	//		{
	//		case 1:	if (ENABLE_Monochrome.get()) bOpenFrag1 = true; break;
	//		case 2:	if (ENABLE_ThreeTones.get()) bOpenFrag2 = true; break;
	//		case 3:	if (ENABLE_HSB.get()) bOpenFrag3 = true; break;
	//
	//#ifdef USE_FX_DELAYS
	//		case 4:	if (ENABLE_Delay.get()) bOpenFrag4 = true; break;
	//		case 5:	if (ENABLE_Echotrace.get()) bOpenFrag5 = true; break;
	//#endif
	//		}
	//	}
	//
	//	refresh_Gui_minimize(true);
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::refresh_Gui_Solos() {
	if (!bSolo) return;

	if (!frag1.active &&
		!frag2.active &&
		!frag3.active
#ifdef USE_FX_DELAYS
		&&
		!frag4.active &&
		!frag5.active
#endif
		)
	{
		bSolo = false;
	}
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::exit()
{
#ifdef USE_ofxPresetsManager
	presetsManager.exit();
#endif

	// callback
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSurfingFxChannel::Changed_params_Control);

	// settings 
#ifndef USE_ofxPresetsManager
	ofxSurfingHelpers::saveGroup(params_Preset, path_GLOBAL_Folder + "/" + path_fileName_Preset);
#endif

	ofxSurfingHelpers::saveGroup(params_Session, path_GLOBAL_Folder + "/" + path_fileName_Session);

	removeKeysListeners();
}

//--

//--------------------------------------------------------------
void ofxSurfingFxChannel::setupStyles() {

	guiManager.clearStyles(); // Clear queued styles if there where previously added, We can do this and changes styles on runtime!

	guiManager.AddStyle(SELECT_Fx, OFX_IM_HSLIDER_SMALL);
	//guiManager.AddStyle(SELECT_Fx, OFX_IM_HSLIDER_MINI);//TODO: fails

	if (guiManager.bMinimize)//fails
	{
		//guiManager.AddStyle(SELECT_Fx, OFX_IM_HIDDEN, 1, false);//fails
		//guiManager.AddStyle(SELECT_Fx_Name, OFX_IM_HIDDEN, 1 , false);
		guiManager.AddStyle(bSolo, OFX_IM_HIDDEN, 2, true);
		guiManager.AddStyle(bReset, OFX_IM_HIDDEN, 2, false);//spacing at end
		guiManager.AddStyle(bAll, OFX_IM_HIDDEN, 2, true);
		guiManager.AddStyle(bNone, OFX_IM_HIDDEN, 2, false, 5);//spacing at end
	}
	else
	{
		//guiManager.AddStyle(SELECT_Fx, OFX_IM_DEFAULT);
		//guiManager.AddStyle(SELECT_Fx_Name, OFX_IM_DEFAULT);
		guiManager.AddStyle(bSolo, OFX_IM_TOGGLE_SMALL_BORDER_BLINK, 2, true);
		guiManager.AddStyle(bReset, OFX_IM_TOGGLE_SMALL, 2, false);//spacing at end
		guiManager.AddStyle(bAll, OFX_IM_TOGGLE_SMALL, 2, true);
		guiManager.AddStyle(bNone, OFX_IM_TOGGLE_SMALL, 2, false, 5);//spacing at end
	}

	guiManager.AddStyle(ENABLE_Monochrome, OFX_IM_TOGGLE_BIG);
	guiManager.AddStyle(ENABLE_ThreeTones, OFX_IM_TOGGLE_BIG);
	guiManager.AddStyle(ENABLE_HSB, OFX_IM_TOGGLE_BIG);
#ifdef USE_FX_DELAYS
	guiManager.AddStyle(ENABLE_Delay, OFX_IM_TOGGLE_BIG);
	guiManager.AddStyle(ENABLE_Echotrace, OFX_IM_TOGGLE_BIG);
#endif
}

//--------------------------------------------------------------
void ofxSurfingFxChannel::ClearStyles() {
	guiManager.clearStyles(); // TODO: -> call from beginWindow/group
}