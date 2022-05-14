#pragma once

#include "ofMain.h"

/*

	NOTE ABOUT THIS EXAMPLE:

	This example includes a Preset Manager
	To run this example out-of-the-box you must enable the presets manager add-on
	uncommenting this line on ofxSurfingFxChannel.h
	#define USE_ofxSurfingPresets

*/


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

		ofxWindowApp windowApp;

		ImageAnimated imageAnimated;
};
