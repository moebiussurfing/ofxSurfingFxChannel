#pragma once

#include "ofMain.h"


//--

// NOTE ABOUT THIS EXAMPLE:

// To run this example out-of-the-box you must enable the presets manager add-on
// uncommenting this line on ofxSurfingFxChannel.h 
// to pick a Presets Manager:
//#define USE_ofxSurfingPresets // 1. Simpler and recommended or

// This example includes a webcam source input to be processed live.
// Should crash if any camera is detected!


//--


#include "ofxSurfingFxChannel.h"

#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

class ofApp : public ofBaseApp
{
	public:

		// Webcam
		ofVideoGrabber vidGrabber;
		void setupWebcamDevice();
		void drawWebcamInfo();
		void exitWebcam();
		void drawWebcam();
		ofParameter <std::string> _dName{ "WEBCAM_DEVICE_NAME", "" };
		int _d;

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed(int key);
		void windowResized(int w, int h);

		ofxSurfingFxChannel channelFx;

		// draw
		void setupScene();
		void drawScene();
		
		// 3D scene
		void draw3D();
		ofEasyCam cam;
		
#ifdef USE_ofxWindowApp
		ofxWindowApp windowApp;
#endif
		int _prim = 1;
		ofBoxPrimitive box;
		ofConePrimitive cone;

		ofTrueTypeFont font;
};
