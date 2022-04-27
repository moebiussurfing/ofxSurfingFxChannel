#pragma once

#include "ofMain.h"


// NOTE ABOUT THIS EXAMPLE:
//
// to run this example out-of-the-box you must disable the presets manager addon
// commenting this line on ofxSurfingFxChannel.h 
//#define INCLUDE_ofxPresetsManager // presets manager


#include "ofxSurfingFxChannel.h"

#include "ofxWindowApp.h" //-> not required

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void windowResized(int w, int h);

		ofxSurfingFxChannel channelFx;

		ofImage img;

		ofxWindowApp windowApp;
};
