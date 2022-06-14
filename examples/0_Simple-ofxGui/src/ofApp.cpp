#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	img.load("image.jpg");

	channelFx.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
	channelFx.begin(); // -> feed your scene inside
	{
		img.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	channelFx.end();
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	channelFx.draw(); // -> draw the processed image
	channelFx.drawGui(); // -> draw GUI
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	channelFx.windowResized(w, h); //-> update required when window size changes
}
