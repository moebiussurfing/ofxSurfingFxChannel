#pragma once

#include "ofMain.h"

// NOTE ABOUT THIS EXAMPLE:
//
// to run this example out-of-the-box you must enable the presets manager add-on
// uncommenting this line on ofxSurfingFxChannel.h :
//#define USE_ofxSurfingPresets // 1. presets manager (simpler and recommended) or
//#define USE_ofxPresetsManager // 2. a bit complex presets manager

#include "ofxSurfingFxChannel.h"

#include "ofxWindowApp.h" //-> not required

class ofApp : public ofBaseApp
{
	public:

		void setup();
		void update();
		void draw();
		void windowResized(int w, int h);

		ofxSurfingFxChannel channelFx;

		ofImage img;

		ofxWindowApp windowApp;
};
