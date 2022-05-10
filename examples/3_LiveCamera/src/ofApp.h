#pragma once

#include "ofMain.h"


//--

/*

	NOTE ABOUT THIS EXAMPLE:

	To run this example out-of-the-box you must enable the presets manager add-on
	uncommenting this line on ofxSurfingFxChannel.h
	to enable a Presets Manager:
	#define USE_ofxSurfingPresets
	USE__OFX_SURFING_PRESETS__OFX_SURFING_PLAYER must be uncommented to.
	To enable the player add-on.

	This example includes a webcam source input to be processed live.
	Could crash if any camera is not detected!

*/

//--


#include "ofxSurfingFxChannel.h"

#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void windowResized(int w, int h);

	// draw
	void setupScene();
	void drawScene();

	ofxSurfingFxChannel channelFx;

	//--

	// Webcam
	ofVideoGrabber vidGrabber;
	void setupWebcamDevice();
	void drawInfo();
	void exitWebcam();
	void drawWebcam();
	ofParameter <std::string> deviceName{ "WEBCAM_DEVICE_NAME", "" };
	int _deviceIndex;

	// 3D scene
	void draw3D();
	ofEasyCam cam;
	int _prim = 1;
	ofBoxPrimitive box;
	ofConePrimitive cone;

	ofTrueTypeFont font;

#ifdef USE_ofxWindowApp
	ofxWindowApp windowApp;
#endif
};
