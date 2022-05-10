#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetBackgroundColor(0);
	imageAnimated.setup("assets/image.jpg");

	channelFx.setup();
}

//--------------------------------------------------------------
void ofApp::update() 
{
	channelFx.begin(); //-> feed your scene inside
	{
		imageAnimated.draw();
	}
	channelFx.end();
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	channelFx.draw(); //-> draw the processed image
	channelFx.drawGui(); //-> draw gui
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	channelFx.windowResized(w, h); //-> update required when window size changes
}
