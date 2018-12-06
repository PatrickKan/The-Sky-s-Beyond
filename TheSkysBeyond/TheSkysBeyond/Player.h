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
			addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		}

		void setupPlayer(int mouseX, int mouseY)
		{
			//addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY);
		}

		int takeDamage();

};