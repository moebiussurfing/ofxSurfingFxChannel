#pragma once

#include "ofMain.h"

// NOTE ABOUT THIS EXAMPLE:
//
// to run this example out-of-the-box you must enable the presets manager addon
// un-commenting this line on ofxSurfingFxChannel.h 
//#define INCLUDE_ofxPresetsManager		// presets manager
//
// and you have to disable both of the two implemented gui's by commenting:
//#define INCLUDE_ofxGuiExtended2	// better gui
//#define INCLUDE_ofxGui	// simpler gui

#include "ofxSurfingFxChannel.h"

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
};
