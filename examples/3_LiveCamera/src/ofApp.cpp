#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	setupScene();

	//--

	// Webcam
	setupWebcamDevice();

	//--

	channelFx.setup();
}

//--------------------------------------------------------------
void ofApp::update() {
	vidGrabber.update();

	//-

	channelFx.begin();
	{
		drawScene();
	}
	channelFx.end();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetBackgroundColor(0);

	//--

	channelFx.draw();
	channelFx.drawGui();

	//--

	drawInfo();
}

//--------------------------------------------------------------
void ofApp::exit()
{
	exitWebcam();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Prims

	if (key == 'p' || key == 'P') {
		_prim++; if (_prim == 2) _prim = 0;
	}

	//--
	
	// Webcam

	// select cam device

	if (key == 'd' || key == 'D') {
		auto _devs = vidGrabber.listDevices();
		_deviceIndex++;
		if (_deviceIndex > _devs.size() - 1) _deviceIndex = 0;
		deviceName = _devs[_deviceIndex].deviceName;

		vidGrabber.close();
		vidGrabber.setDeviceID(_deviceIndex);
		vidGrabber.setup(1920, 1080);
	}

	// restart device

	if (key == 'r' || key == 'R') {
		vidGrabber.close();
		vidGrabber.setDeviceID(_deviceIndex);
		vidGrabber.setup(1920, 1080);
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--

// Scene

//--------------------------------------------------------------
void ofApp::setupScene() {
	string str = "overpass-mono-bold.otf";
	string pathFont = "assets/fonts/" + str;// /data/assets
	bool b = font.load(pathFont, 10);
	if (!b) font.load(OF_TTF_MONO, 10);

	//--

	// Prims

	float _size = 400;

	// box
	box.set(_size);
	box.setPosition(0, _size * 0.5, 0);
	box.setResolution(1);

	// cone
	float _cSize = 1.62 * _size;
	cone.set(_size, _cSize, 3, 1, 1);
	cone.setPosition(0, _cSize * 0.5, 0);
	cone.tiltDeg(180);

	//--

	// Camera

	cam.enableOrtho();
	cam.setFarClip(10000);
	cam.setNearClip(-10000);
	cam.tiltDeg(30);
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	drawWebcam();

	draw3D();
}

//--

// Prims

//--------------------------------------------------------------
void ofApp::draw3D() {
	ofEnableDepthTest();
	//ofEnableAlphaBlending();

	ofPushStyle();

	cam.begin();
	{
		//-

		int _speed = 60 * 10;
		ofRotateYDeg(ofMap(ofGetFrameNum() % _speed, 0, _speed, 0, 359));

		//-

		//prims
		ofFill();
		ofSetColor(0, 64);
		//ofSetColor(255, 64);
		//ofSetColor(0);
		if (_prim == 0) {
			box.drawFaces();
		}
		else if (_prim == 1) {
			cone.drawFaces();
		}

		//-

		// wire prims
		ofNoFill();
		ofSetColor(255, 200);
		//ofSetColor(0, 200);
		//ofSetColor(255);
		ofSetLineWidth(2.);
		if (_prim == 0) {
			box.drawWireframe();
		}
		else if (_prim == 1) {
			cone.drawWireframe();
		}
	}
	cam.end();

	ofPopStyle();

	//ofDisableAlphaBlending();
	ofDisableDepthTest();
}

//--

// Webcam

//--------------------------------------------------------------
void ofApp::setupWebcamDevice() {

	// load settings
	ofXml _xml;
	bool _isLoaded;
	_isLoaded = _xml.load("settings.xml");
	ofDeserialize(_xml, deviceName);
	ofLogNotice(__FUNCTION__) << _xml.toString();

	// start
	auto _devices = vidGrabber.listDevices();
	_deviceIndex = -1;
	if (_isLoaded) {
		for (int i = 0; i < _devices.size(); i++) {
			if (_devices[i].deviceName == deviceName.get()) {
				_deviceIndex = i;
				ofLogNotice(__FUNCTION__) << "device name:\t" << deviceName.get();
				ofLogNotice(__FUNCTION__) << "device index:\t" << _deviceIndex;
			}
		}
	}
	if (_deviceIndex == -1) {//error. try to load first device...
		_deviceIndex = 0;//select cam device
		deviceName = _devices[_deviceIndex].deviceName;
	}

	// start device
	vidGrabber.setVerbose(true);
	vidGrabber.setDeviceID(_deviceIndex);
	vidGrabber.setup(1920, 1080);
}
//--------------------------------------------------------------
void ofApp::drawInfo()
{
	// display device name
	string str;
	str += deviceName.get();
	str += " " + ofToString(vidGrabber.isInitialized() ? "ON" : "OFF");
	str += " #" + ofToString(_deviceIndex);
	str += "\n";
	str += "\n D: NEXT DEVICE";
	str += "\n R: RESTART DEVICE";
	str += "\n P: NEXT PRIM";

	int xx = 30;
	int yy = 40;

	if (!font.isLoaded()) ofDrawBitmapStringHighlight(str, xx, yy);
	else {
		ofPushStyle();

		// bbox
		ofSetColor(0, 150);
		ofFill();
		ofRectangle _r(font.getStringBoundingBox(str, xx, yy));
		int pad = 20;
		_r.setWidth(_r.getWidth() + pad);
		_r.setHeight(_r.getHeight() + pad);
		_r.setX(_r.getPosition().x - pad / 2.);
		_r.setY(_r.getPosition().y - pad / 2.);
		//_r.scaleFromCenter(1.05, 1.5);
		ofDrawRectRounded(_r, 5.);

		// text
		ofSetColor(255);
		ofNoFill();
		font.drawString(str, xx, yy);

		ofPopStyle();
	}
}
//--------------------------------------------------------------
void ofApp::drawWebcam() {
	ofRectangle r(0, 0, vidGrabber.getWidth(), vidGrabber.getHeight());
	r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_CENTER);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_STRETCH_TO_FILL);
	//r.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FILL);
	vidGrabber.draw(r.x, r.y, r.width, r.height);
}
//--------------------------------------------------------------
void ofApp::exitWebcam() {
	ofXml _xml;
	ofSerialize(_xml, deviceName);
	_xml.save("settings.xml");
}