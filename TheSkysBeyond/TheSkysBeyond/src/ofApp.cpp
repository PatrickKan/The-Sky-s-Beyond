#include "ofApp.h"
#include "..//Player.h"
#include <iostream>
#include <iomanip>

//--------------------------------------------------------------
void ofApp::setup()
{
	state = GameState::LOADING;

	ofSetVerticalSync(true);
	ofBackground(ofColor(255, 255, 255));
	ofSetLogLevel(OF_LOG_NOTICE);

	//Data loaded from bin/data
	background.load("space_background2.png");
	
	soundPlayer.load("TheAbyss.mp3");
	soundPlayer.play();
	soundPlayer.setMultiPlay(true);

	shootSoundPlayer.load("ShootSound.mp3");
	shootSoundPlayer.setMultiPlay(true);
	shootSoundPlayer.setVolume(0.09);

	infoFont.load("Roboto-Regular.ttf", 16);
	scoreFont.load("Roboto-Regular.ttf", 20);

	box2d.init();
	box2d.setGravity(0, 0);
	box2d.setFPS(60.0);
	box2d.registerGrabbing();

	// register the listener to get events
	box2d.enableEvents();

	ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);

	createPlayer();
}

void ofApp::createPlayer()
{
	players.push_back(shared_ptr<Player>(new Player(ofGetWindowWidth()/15, ofGetWindowHeight()/2)));
	Player* player = players.back().get();
	player->create(box2d.getWorld());

	createPlayerBoosters();
}

void ofApp::createPlayerBoosters()
{
	float w = 4;
	float h = 4;
	auto player = players[0];
	ofVec2f position = player->getPosition();

	boosters.push_back(std::make_shared<ofxBox2dRect>());
	boosters.back()->setPhysics(10000000000000000000000000.0, 0.53, 0.1);
	boosters.back()->setup(box2d.getWorld(), position.x - 10 - 2, position.y + 4, w, h);

	boosters.push_back(std::make_shared<ofxBox2dRect>());
	boosters.back()->setPhysics(10000000000000000000000000.0, 0.53, 0.1);
	boosters.back()->setup(box2d.getWorld(), position.x - 10 - 2, position.y - 4, w, h);
}

//--------------------------------------------------------------
void ofApp::update()
{
	box2d.update();

	auto player = players[0];

	//Continue to play music if not playing
	if (!soundPlayer.isPlaying())
	{
		soundPlayer.play();
	}

	if (player->currentHealth() <= 0)
	{
		state = GameState::LOADING;
		prevScore = int(playerXPos);
		soundPlayer.stop();
		resetGame();
	}

	//Game is in play
	if (state == GameState::PLAYING && (players[0] != nullptr))
	{
		playGame();
		updates++;
	}

	createGravity();

	// remove shapes offscreen
	ofRemove(boxes, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(customParticles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(triangles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(planets, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(shotCircles, ofxBox2dBaseShape::shouldRemoveOffScreen);
}

void ofApp::playGame()
{
	
	ofVec2f currPos = players[0]->getPosition();
	int xPos = currPos.x;
	int yPos = currPos.y;

	float angle = atan2(yPos - mouseY, mouseX - xPos);
	players[0]->setVelocity(0, -playerVelocity * sin(angle));

	for (auto booster : boosters)
	{
		booster->setVelocity(0, -playerVelocity * sin(angle));
	}

	//Example moving x velocity
	//players[0]->setVelocity(playerVelocity*cos(angle), -playerVelocity * sin(angle));

	scrollVelocity = playerVelocity;
	playerXPos += scrollVelocity * 0.1;

	//Player moves faster as game progresses
	playerVelocity += 0.05/(1+exp(updates/1000));
	
	//Add planets and circles randomly during update
	addPlanetObstacle();
	addCircleObstacle();
}

void ofApp::createGravity()
{
	//Planet gravity
	//For planet force, gravity should be equivalent to 1/r^2, but not too fast when approaching zero (check w/ if for r > 0.01)
	for (auto planet : planets)
	{
		float gravity = 0;

		ofVec2f planetPos = planet->getPosition();
		int planetX = planetPos.x;
		int planetY = planetPos.y;

		ofVec2f planetVel = planet->getVelocity();
		planet->setVelocity(-scrollVelocity, planetVel.y);

		for (auto circle : circles)
		{
			gravity = computeGravity(circle->getPosition(), planet);
			circle->addAttractionPoint(planetX, planetY, gravity);
		}
		for (auto particle : customParticles)
		{
			gravity = computeGravity(particle->getPosition(), planet);
			particle->addAttractionPoint(planetX, planetY, gravity);
		}
		for (auto box : boxes)
		{
			gravity = computeGravity(box->getPosition(), planet);
			box->addAttractionPoint(planetX, planetY, gravity);
		}
		for (auto triangle : triangles)
		{
			gravity = computeGravity(triangle->getPosition(), planet);
			triangle->addAttractionPoint(planetX, planetY, gravity);
		}
	}
}

void ofApp::addCircleObstacle()
{
	float chance = ofRandom(1, 100);

	//Chance of creating obstacle every update based on how fast player is going
	if (chance <= scrollVelocity / 5)
	{
		float rad = ofRandom(10, 20);		// a random radius 10px - 20px
		int randYCoord = ofRandom(0 + rad, ofGetWindowHeight() - rad);

		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(1.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), ofGetWindowWidth()-rad, randYCoord, rad);
		circles.back()->setVelocity(-scrollVelocity, 0);
	}
}

void ofApp::addPlanetObstacle()
{
	float chance = ofRandom(1, 100);

	//Chance of creating obstacle every update based on how fast player is going
	if (chance <= scrollVelocity / 15)
	{
		float rad = ofRandom(50, 70);		// a random radius 4px - 20px
		int randYCoord = ofRandom(0 + rad, ofGetWindowHeight() - rad);

		planets.push_back(std::make_shared<ofxBox2dCircle>());
		planets.back()->setPhysics(1000000000.0, 0, 0.1);
		planets.back()->setup(box2d.getWorld(), ofGetWindowWidth() - rad, randYCoord, rad);
	}
}

//Takes in planet position and radius as parameter
float ofApp::computeGravity(ofVec2f currPos, ofVec2f planetPos, int planetRad)
{
	int xPos = currPos.x;
	int yPos = currPos.y;

	int planetX = planetPos.x;
	int planetY = planetPos.y;

	//Hypot taken from https://en.cppreference.com/w/cpp/numeric/math/hypot
	float distance = std::hypot(xPos - planetX, yPos - planetY);

	float gravity = 0;

	//Double check to not divide by 0
	if (distance != 0)
	{
		gravity = 3000 * float(planetRad) / pow(distance, 2);
	}
	return gravity;
}

//Takes in planet as parameter
float ofApp::computeGravity(ofVec2f currPos, std::shared_ptr<ofxBox2dCircle> planet)
{
	int planetRad = planet->getRadius();

	int xPos = currPos.x;
	int yPos = currPos.y;

	ofVec2f planetPos = planet->getPosition();
	int planetX = planetPos.x;
	int planetY = planetPos.y;

	//Hypot taken from https://en.cppreference.com/w/cpp/numeric/math/hypot
	float distance = std::hypot(xPos - planetX, yPos - planetY);

	float gravity = 0;

	//Double check to not divide by 0
	if (distance != 0)
	{
		gravity = 1000 * float(planetRad) / pow(distance, 2);
	}

	return gravity;
}


//--------------------------------------------------------------
void ofApp::draw()
{
	//Three backgrounds drawn of the same image to simulate an infinite scrolling background

	int windowWidth = ofGetWindowWidth();
	int imageXPos = (int(-playerXPos) % int(4 * ofGetWindowWidth()));

	background.draw(imageXPos, 0, 2 * windowWidth, ofGetWindowHeight());
	background.draw(imageXPos + (2 * windowWidth) - 1, 0, 2 * windowWidth, ofGetWindowHeight());
	background.draw(imageXPos + (4 * windowWidth) - 1, 0, 2 * windowWidth, ofGetWindowHeight());

	for (auto circle : circles)
	{
		ofFill();
		ofSetHexColor(0x90d4e3);
		circle->draw();
	}

	for (auto box : boxes)
	{
		ofFill();
		ofSetHexColor(0xe63b8b);
		box->draw();
	}

	for (auto customParticle : customParticles)
	{
		customParticle->draw();
	}

	for (auto triangle : triangles)
	{
		ofFill();
		ofSetHexColor(0xEE82EE);
		triangle->draw();
	}

	for (auto planet : planets)
	{
		ofFill();
		ofSetHexColor(0xcc3333);
		planet->draw();
	}

	for (auto player : players)
	{
		player->draw();
	}

	for (auto booster : boosters)
	{
		booster->draw();
	}

	for (auto shotCircle : shotCircles)
	{
		ofFill();
		ofSetHexColor(0xffae19);
		shotCircle->draw();
	}
	
	ofSetHexColor(0xffffff);

	if (state == GameState::LOADING)
	{
		scoreFont.drawString("The Sky's Beyond", ofGetWindowWidth() / 3, ofGetWindowHeight() / 5);

		string info = "";
		info += "Dodge the obstacles in space! The ship follows your mouse.\n";
		info += "Press z to shoot missiles (100 available)\n";
		info += "Press x to shoot large cannons (2 available)\n";
		info += "Press spacebar to begin\n\n\n";
		info += "Your last score: " + std::to_string(prevScore) + "\n";
		info += "FPS: " + ofToString(ofGetFrameRate()) + "\n";
		
		infoFont.drawString(info, ofGetWindowWidth() / 3, ofGetWindowHeight() / 3);
	}
	else if (state == GameState::PLAYING)
	{
		infoFont.drawString("FPS: " + ofToString(ofGetFrameRate()) + "\n", ofGetWindowWidth() / 10, 9 * ofGetWindowHeight() / 10);
		scoreFont.drawString("Score: " + std::to_string(int(playerXPos)), 8 * ofGetWindowWidth() / 10, ofGetWindowHeight() / 10);

		string info = "";
		info += "Current Health: " + std::to_string(players[0]->currentHealth()) + "\n";
		info += "Current Velocity: " + std::to_string(playerVelocity) + "\n";
		info += "Current Ammo: " + std::to_string(ammo) + "\n";
		info += "Cannons available: " + std::to_string(cannonAmmo);
		scoreFont.drawString(info, ofGetWindowWidth() / 10, ofGetWindowHeight() / 10);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == ' ') //Space bar used to initiate game, follow mouse up and down related to scroll velocity
	{
		state = GameState::PLAYING;
	}

	if (key == '=') //Create a planet with gravity and varying size
	{
		float r = ofRandom(50, 70);		
		planets.push_back(std::make_shared<ofxBox2dCircle>());
		planets.back()->setPhysics(100000.0, 0, 0.1);
		planets.back()->setup(box2d.getWorld(), mouseX, mouseY, r);

	}

	if (key == 'z' && state == GameState::PLAYING && ammo > 0)
	{
		shootCircle(3, 60, 10);
		ammo--;
	}

	if (key == 'x' && state == GameState::PLAYING && cannonAmmo > 0)
	{
		shootCircle(100000000000000000.0, 75, 50);
		cannonAmmo--;
		
	}

	if (key == 't') ofToggleFullscreen();
}

//Shoots circle by placing it in front of player with an initial forward velocity
void ofApp::shootCircle(float density, int velocity, int radius)
{
	auto player = players[0];

	ofVec2f playerPos = player->getPosition();
	int playerX = playerPos.x;
	int playerY = playerPos.y;
		
	shotCircles.push_back(std::make_shared<ofxBox2dCircle>());
	shotCircles.back()->setPhysics(density, 0, 0);
	shotCircles.back()->setup(box2d.getWorld(), playerX + radius + 22 , playerY , radius);
	shotCircles.back()->setVelocity(velocity, 0.0);

	shootSoundPlayer.play();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//Take damage if contact between player and circle
void ofApp::contactStart(ofxBox2dContactArgs &e)
{
	if (e.a != NULL && e.b != NULL)
	{
		if (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle)
		{
			auto player = players[0];
			player->takeDamage(5);
		}
	}
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e)
{
	if (e.a != NULL && e.b != NULL)
	{
		if (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle)
		{
			//Can add functionality when contact is ended if desired
		}
	}
}

void ofApp::resetGame()
{
	auto world = box2d.getWorld();

	circles.erase(circles.begin(), circles.end());
	planets.erase(planets.begin(), planets.end());
	players.erase(players.begin(), players.end());
	createPlayer();
	boosters.erase(boosters.begin(), boosters.end());
	createPlayerBoosters();
	shotCircles.erase(shotCircles.begin(), shotCircles.end());

	scrollVelocity = 0;
	playerVelocity = 20;
	playerXPos = 0;
	players[0]->resetHealth();
	updates = 0;
	ammo = 100;
	cannonAmmo = 2;
}

//Taken from https://www.geeksforgeeks.org/rounding-floating-point-number-two-decimal-places-c-c/
//to round a float to 2 decimal palces
float roundToTwo(float var)
{
	// we use array of chars to store number 
	// as a string. 
	char str[40];

	// Print in string the value of var  
	// with two decimal point 
	sprintf(str, "%.2f", var);

	// scan string value in var  
	sscanf(str, "%f", &var);

	return var;
}