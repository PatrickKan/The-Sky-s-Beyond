#pragma once
#include "src//ofApp.h"
#include "Player.h"
#include<iostream>
#include<string>
#include <vector>
#include <utility>
using std::string;
using std::vector;

TEST_CASE("Gravity Tests")
{
	ofApp test;

	SECTION("Gravity calculating correctly")
	{
		ofVec2f playerPosition(10, 10);
		ofVec2f planetPosition(1000, 1000);
		int planetRad = 100;

		REQUIRE(test.computeGravity(playerPosition, planetPosition, planetRad) > 0);
	}

	SECTION("Strength based only on 1/distance^2, regardless of direction")
	{
		ofVec2f playerPosition(500, 500);

		ofVec2f planet1Position(100, 100);
		ofVec2f planet2Position(900, 900);

		int planetRad = 100;

		float p1Gravity = test.computeGravity(playerPosition, planet1Position, planetRad);
		float p2Gravity = test.computeGravity(playerPosition, planet2Position, planetRad);

		REQUIRE(p1Gravity == p2Gravity);
	}

	//Not possible usually, however engine could glitch with collisions 
	SECTION("Same position as planet edge case")
	{
		ofVec2f playerPosition(100, 100);
		ofVec2f planetPosition(100, 100);
		int planetRad = 50;

		//Gravity set to zero instead of dividing by 0
		REQUIRE(test.computeGravity(playerPosition, planetPosition, planetRad) == 0);
	}
}

//Unable to test creating objects because there must be a valid world present
TEST_CASE("Creating Objects")
{
	ofApp test;
	
	//Needs world to be created in order to test
	SECTION("Player Creation")
	{
		//test.setup();
		//REQUIRE(test.players.size() == 1);
	}

	SECTION("Shoot Circle Creation")
	{
		//test.keyPressed('z');
		//REQUIRE(test.circles.size() > 0);
	}
}

TEST_CASE("Player Tests")
{
	Player testPlayer(0,0);

	SECTION("Taking damage")
	{
		int currentHealth = testPlayer.currentHealth();
		testPlayer.takeDamage(10);
		REQUIRE(testPlayer.currentHealth() == currentHealth - 10);
	}
}

