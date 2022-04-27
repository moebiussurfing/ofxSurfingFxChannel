#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetBackgroundColor(0);
	img.load("image.jpg");

	channelFx.setup();
}

//--------------------------------------------------------------
void ofApp::update() {
	channelFx.begin(); // -> feed your scene inside
	{
		// animated zoom
		float scale = (ofGetFrameNum() % 600) / 600.f;
		ofScale(ofMap(scale, 0, 1, 1, 1.2));
		img.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	channelFx.end();
}

//--------------------------------------------------------------
void ofApp::draw() {
	channelFx.draw(); // -> draw the processed image
	channelFx.drawGui(); //-> draw gui
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	channelFx.windowResized(w, h); //-> update required when window size changes
}
