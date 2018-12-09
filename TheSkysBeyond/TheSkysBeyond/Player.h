#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"

//Player class is a triangle with health 
class Player : public ofxBox2dPolygon
{
	int health = 100;

	public:
		Player(int xPos, int yPos)
		{
			addTriangle(ofPoint(xPos-10, yPos + 10), ofPoint(xPos-10, yPos - 10), ofPoint(xPos + 20, yPos));
			setPhysics(10000000000000000000000000.0, 0.0, 0.0);

			float w = ofRandom(10, 30);
			float h = ofRandom(10, 30);
			
		}

		void draw()
		{
			ofFill();
			ofSetHexColor(0xEE82EE);
			ofxBox2dPolygon::draw(); //Parent method to draw
		}

		int takeDamage(int damage);
		int currentHealth();
		
};