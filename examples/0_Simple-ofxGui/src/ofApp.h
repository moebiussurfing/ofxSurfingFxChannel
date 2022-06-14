#pragma once

#include "ofMain.h"


// NOTE ABOUT THIS EXAMPLE:
//
// To run this example out-of-the-box you must 
// Include the ofxGui uncommenting USE_ofxGui
// on ofxSurfingFxChannel.h 
// and disable both ImGui add-ons: ofxImGui and ofxImGuiSurfing.
// Disable the presets manager addon too,
// commenting this line 
//#define INCLUDE_ofxPresetsManager // presets manager


#include "ofxSurfingFxChannel.h"

class ofApp : public ofBaseApp
{
	public:

		void setup();
		void update();
		void draw();
		void windowResized(int w, int h);

		ofxSurfingFxChannel channelFx;

		ImageAnimated img; // -> From ofxSurfing_Widgets.h / ofxSurfingHelpers
};
