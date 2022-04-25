#pragma once

#include "ofMain.h"


// NOTE ABOUT THIS EXAMPLE:
//
// to run this example out-of-the-box you must disable the presets manager addon
// commenting this line on ofxSurfingFxChannel.h 
//#define INCLUDE_ofxPresetsManager // presets manager


#include "ofxSurfingFxChannel.h"
#include "ofxWindowApp.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed(int key);
		void windowResized(int w, int h);

		ofxSurfingFxChannel channelFx;

		ofImage img;

		ofxWindowApp windowApp;
};
