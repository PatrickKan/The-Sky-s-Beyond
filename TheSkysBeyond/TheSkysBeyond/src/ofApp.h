#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"

// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle
{

public:
	CustomParticle()
	{
	}

	void draw()
	{
		ofPushMatrix();
		ofTranslate(getPosition());
		ofSetColor(color.r, color.g, color.b);
		ofFill();
		ofDrawCircle(0, 0, getRadius());
		ofPopMatrix();
	}

	ofColor color;

};

class Planet : public ofxBox2dCircle
{
	int radius;
	int gravity;

public:

	Planet()
	{

	}

	int GetGravity();
};

// -------------------------------------------------
class ofApp : public ofBaseApp
{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	float ComputeGravity(ofVec2f position, ofVec2f planet_pos, int planet_rad);

	// this is the function for contacts
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);


	float px = 0;
	float py = 0;
	bool bDrawLines = false;
	bool bMouseForce = false;
	bool followMouse = false;
	bool mouseDown = false;

	float playerVelocity = 5;
	float scrollVelocity = 0;
	float playerXPos = 0;

	int backgroundState = 0;
	
	int fuel = 1000;

	ofImage background;
	ofCamera view;

	ofxBox2d box2d; // the box2d world
	ofPolyline drawing; // we draw with this first
	ofxBox2dEdge edgeLine; // the box2d edge/line shape (min 2 points)
	std::vector<std::shared_ptr<ofxBox2dCircle>> circles; // default box2d circles
	std::vector<std::shared_ptr<ofxBox2dRect>> boxes; // default box2d rects
	std::vector<std::shared_ptr<ofxBox2dPolygon>> triangles;
	std::vector<std::shared_ptr<CustomParticle>> customParticles; // this is a custom particle the extends a cirlce

	std::vector<std::shared_ptr<ofxBox2dCircle>> planets;

	//TODO:
	//Implement sliding by having velocity go back (also slide background slowly as traverse through level)
	//Add contact listener (to know when the player is hit by another object)
	//if mouse pressed, engage booster
};