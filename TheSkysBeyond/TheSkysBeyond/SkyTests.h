#pragma once
#include "src//ofApp.h"
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

		REQUIRE(test.ComputeGravity(playerPosition, planetPosition, planetRad) > 0);
	}

	SECTION("Strength based only on 1/distance^2, regardless of direction")
	{
		ofVec2f playerPosition(500, 500);

		ofVec2f planet1Position(100, 100);
		ofVec2f planet2Position(900, 900);

		int planetRad = 100;

		float p1Gravity = test.ComputeGravity(playerPosition, planet1Position, planetRad);
		float p2Gravity = test.ComputeGravity(playerPosition, planet2Position, planetRad);

		REQUIRE(p1Gravity == p2Gravity);
	}

	//Not possible usually, however engine could glitch with collisions 
	SECTION("Same position as planet edge case")
	{
		ofVec2f playerPosition(100, 100);
		ofVec2f planetPosition(100, 100);
		int planetRad = 50;

		//Gravity set to zero instead of dividing by 0
		REQUIRE(test.ComputeGravity(playerPosition, planetPosition, planetRad) == 0);
	}

}