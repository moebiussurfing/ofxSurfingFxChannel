#pragma once

#include "ofMain.h"


// NOTE ABOUT THIS EXAMPLE:
//
// To run this example out-of-the-box you must disable the presets manager add-on
// commenting this line on ofxSurfingFxChannel.h: 
// //#define INCLUDE_ofxPresetsManager


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

		ImageAnimated img; // -> From ofxSurfing_Widgets.h / ofxSurfingHelpers

		ofxWindowApp windowApp;
};
