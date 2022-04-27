ofxSurfingFxChannel
=============================
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Stage: beta](https://img.shields.io/badge/-alpha-red)

# Overview

**ofxSurfingFxChannel** is an **openFrameworks** add-on to live image processing with common effects:  

HSB, Contrast, Brightness, Saturation, Hue, Monochrome, Tree Tones Colors, and Delays.

Powered by [ofxDotFrag](https://github.com/npisanti/ofxDotFrag) from [npisanti](https://github.com/npisanti).

## Screenshots

![image](/readme_images/Capture3.PNG?raw=true "image")

## Features
- Channel Strip to do image processing with common useful effects.
- Monochrome, Three Tones, Hue/Saturation/Brightness/Contrast, Delay, and Echo Trace effects.
- Powered by the shaders of [ofxDotFrag](https://github.com/npisanti/ofxDotFrag).
- Store/Recall settings.
- Optional Presents Manager.
- Implemented GUI's workflow for **ofxImGui** or **ofxGui**.

## Usage
Check examples.  

### ofApp.h
```.cpp
#include "ofxSurfingFxChannel.h"
ofxSurfingFxChannel channelFx;
```

### ofApp.cpp
```.cpp
ofApp::setup()
{
	channelFx.setup();
}

ofApp::update()
{
	channelFx.begin();
	{
		// draw here!
	}
	channelFx.end();
}

ofApp::draw()
{
	channelFx.draw();
	channelFx.drawGui();
}
```

<details>
  <summary>Dependencies</summary>
  <p>

* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
* [ofxSurfingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)  
* [ofxImGui](https://github.com/Daandelange/ofxImGui/) [_Fork_]  

The below add-ons are already packed into **OF_ADDON/libs/**.  
No need to add them manually with the **OF PROJECT GENERATOR**:    
* [ofxDotFrag](https://github.com/npisanti/ofxDotFrag) 

Optional:  
* [ofxSurfingPresets](https://github.com/moebiussurfing/ofxSurfingPresets)  
* ofxGui [_Core_]  
  </p>
</details>

<details>
  <summary>Tested Systems</summary>
  <p>

  - **Windows 10** / **VS 2017** / **OF ~0.11**
  </p>
</details>

## Author
An addon by **@moebiusSurfing**  
*( ManuMolina ) 2019-2021*  

### Contact / Follow
<p>
<a href="mailto:moebiussurfing@gmail.com" target="_blank">Email</a> |
<a href="https://twitter.com/moebiusSurfing/" rel="nofollow">Twitter</a> | 
<a href="https://www.youtube.com/moebiusSurfing" rel="nofollow">YouTube</a> | 
<a href="https://www.instagram.com/moebiusSurfing/" rel="nofollow">Instagram</a> | 
<a href="https://github.com/moebiussurfing" target="_blank">GitHub</a> 
</p>

### License
MIT License