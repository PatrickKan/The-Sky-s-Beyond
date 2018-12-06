#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"
#include "src\ofApp.h"

class Player : public ofxBox2dPolygon
{
	int health;

	public:
		Player(int mouseX, int mouseY)
		{
			addTriangle(ofPoint(mouseX-10, mouseY + 10), ofPoint(mouseX-10, mouseY - 10), ofPoint(mouseX + 20, mouseY));
			setPhysics(100000000.0, 0.0, 0.0);
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