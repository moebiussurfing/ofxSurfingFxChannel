ofxSurfingFxChannel
=============================
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Stage: beta](https://img.shields.io/badge/-alpha-red)

# Overview

**ofxSurfingFxChannel** is an **openFrameworks** add-on to live image processing with common effects:  

Monochrome, HSB, Hue, Saturation, Brightness, Contrast, Tree Tones Colors, Delay and Echo Trace.

Powered by [ofxDotFrag](https://github.com/npisanti/ofxDotFrag) from [npisanti](https://github.com/npisanti).

![](/examples/1_BasicNoPresets/Capture.PNG)

[Screenshots](https://github.com/moebiussurfing/ofxSurfingFxChannel/blob/master/examples/README.md)  

## Features
- Channel Strip to image processing with common Effects.
- Auto Store/Recall Settings.
- Optional Presets Manager.
- Implemented GUI with **ofxImGui** or **ofxGui**.

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

* [ofxSurfingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)  
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
* [ofxImGui](https://github.com/Daandelange/ofxImGui/) / _Optional. FORK from @Daandelange_  
* [ofxSurfingPresets](https://github.com/moebiussurfing/ofxSurfingPresets) / _Optional._  
* ofxGui / _Optional. Core_  

The below add-ons are already packed into **OF_ADDON/libs/**.  
No need to add them manually with the **OF PROJECT GENERATOR**:    
* [ofxDotFrag](https://github.com/npisanti/ofxDotFrag) 
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
*( ManuMolina ) 2019-2022*  

### Contact / Tune
<p>
<a href="mailto:moebiussurfing@gmail.com" target="_blank">Email</a> |
<a href="https://twitter.com/moebiusSurfing/" rel="nofollow">Twitter</a> | 
<a href="https://www.youtube.com/moebiusSurfing" rel="nofollow">YouTube</a> | 
<a href="https://www.instagram.com/moebiusSurfing/" rel="nofollow">Instagram</a> | 
<a href="https://github.com/moebiussurfing" target="_blank">GitHub</a> 
</p>

### License
MIT License