#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"
#include "..//Player.h"

// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle
{

public:
	CustomParticle()
	{
	}

	void draw()
	{
		//Calls these methods in the draw method
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

enum class GameState
{
	LOADING = 0,
	PLAYING = 1,
	DEAD = 2
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

	void playGame();

	void createGravity();
	float computeGravity(ofVec2f position, ofVec2f planet_pos, int planet_rad);
	float computeGravity(ofVec2f currPos, std::shared_ptr<ofxBox2dCircle> planet);
	
	// this is the function for contacts
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);

	void createPlayer();
	void createPlayerBoosters();
	void shootCircle(float density, int velocity, int radius);

	void addCircleObstacle();
	void addBlockObstacle();
	void addTriangleObstacle();
	void addPlanetObstacle();

	void resetGame();

	ofSoundPlayer soundPlayer;
	ofSoundPlayer shootSoundPlayer;

	GameState state;

	//Font to display text
	ofTrueTypeFont infoFont;
	ofTrueTypeFont scoreFont;

	float px = 0;
	float py = 0;
	bool bDrawLines = false;
	bool bMouseForce = false;
	bool followMouse = false;
	bool mouseDown = false;

	float playerVelocity = 20;
	float scrollVelocity = 0;
	float playerXPos = 0;

	int backgroundState = 0;
	
	int ammo = 100;
	int cannonAmmo = 2;

	int updates = 0;
	int prevScore = 0;

	ofImage background;

	ofxBox2d box2d; // the box2d world
	ofPolyline drawing; // we draw with this first
	ofxBox2dEdge edgeLine; // the box2d edge/line shape (min 2 points)
	std::vector<std::shared_ptr<ofxBox2dCircle>> circles; // default box2d circles
	std::vector<std::shared_ptr<ofxBox2dRect>> boxes; // default box2d rects
	std::vector<std::shared_ptr<ofxBox2dPolygon>> triangles;
	std::vector<std::shared_ptr<CustomParticle>> customParticles; // this is a custom particle that extends a circle
	std::vector<std::shared_ptr<ofxBox2dCircle>> planets;

	std::vector<std::shared_ptr<Player>> players;
	std::vector<std::shared_ptr<ofxBox2dRect>> boosters;
	std::vector<std::shared_ptr<ofxBox2dCircle>> shotCircles;
	std::vector<std::shared_ptr<ofxBox2dCircle>> cannons;
};


//Taken from https://www.geeksforgeeks.org/rounding-floating-point-number-two-decimal-places-c-c/
//to round a float to 2 decimal palces
float roundToTwo(float var);
