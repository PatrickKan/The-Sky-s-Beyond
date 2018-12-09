#include "ofApp.h"
#include "..//Player.h"

//Build map with array of circles/points/blocks
//Underscore for private vars
//for each loops
//draw images for planets and spaceship

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(true);
	ofBackground(ofColor(255, 255, 255));
	ofSetLogLevel(OF_LOG_NOTICE);

	//Data loaded from bin/data
	background.load("space_background2.png");
	
	soundPlayer.load("TheAbyss.mp3");
	soundPlayer.play();
	soundPlayer.setMultiPlay(true);

	bMouseForce = false;
	followMouse = false;

	box2d.init();
	box2d.setGravity(0, 0);
	//box2d.createGround();
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

	//Continue to play music if not playing
	if (!soundPlayer.isPlaying())
	{
		soundPlayer.play();
	}

	if (followMouse && (players[0] != nullptr))
	{
		int velocity = 5;

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
	}

	if (mouseDown && fuel > 0)
	{
		playerVelocity += 0.05;
		fuel--;
	}

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

	//Player is moving (game is in play)
	if (followMouse)
	{	
		//Add planets and circles randomly during update
		addPlanetObstacle();
		addCircleObstacle();
		//addBlockObstacle();
	}

	// remove shapes offscreen
	ofRemove(boxes, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(customParticles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(triangles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(planets, ofxBox2dBaseShape::shouldRemoveOffScreen);
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

void ofApp::addBlockObstacle()
{
	float chance = ofRandom(1, 100);

	//Chance of creating obstacle every update based on how fast player is going
	if (chance <= scrollVelocity / 5)
	{
		float width = ofRandom(10, 20);		
		float height = ofRandom(10, 20);
		int randYCoord = ofRandom(0 + height, ofGetWindowHeight() - height);

		boxes.push_back(std::make_shared<ofxBox2dRect>());
		boxes.back()->setPhysics(3.0, 0.53, 0.1);
		boxes.back()->setup(box2d.getWorld(), ofGetWindowWidth() - width, randYCoord, width, height);
		boxes.back()->setVelocity(-scrollVelocity, 0);
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

		std::cout << "Planet created\n";
	}

}

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


	ofNoFill();
	ofSetHexColor(0xffe6e6);
	if (drawing.size() == 0)
	{
		edgeLine.updateShape();
		edgeLine.draw();
	}
	else drawing.draw();

	string info = "";
	info += "Press spacebar to begin\n";
	info += "Hold down mouse to boost. Current fuel: " + to_string(fuel) + "\n";
	info += "Press [c] for circles\n";
	info += "Press [b] for blocks\n";
	info += "Press [q] for triangles\n";
	info += "Press [=] to create a planet with gravity\n";
	info += "MouseX: " + std::to_string(mouseX) + "\n";
	info += "MouseY: " + std::to_string(mouseY) + "\n";
	info += "Total Bodies: " + ofToString(box2d.getBodyCount()) + "\n";
	info += "Total Joints: " + ofToString(box2d.getJointCount()) + "\n\n";
	info += "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	info += "Current Health: " + std::to_string(players[0]->currentHealth()) + "\n";
	info += "Score: " + std::to_string(int(playerXPos)) + "\n";
	info += "Velocity: " + std::to_string(scrollVelocity) + "\n";
	ofSetHexColor(0xffffff);
	ofDrawBitmapString(info, 30, 30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'c')
	{
		float r = ofRandom(4, 20);		// a random radius 4px - 20px
		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(1.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), mouseX, mouseY, r);
		circles.back()->setVelocity(-scrollVelocity, 0);
	}

	if (key == 'b')
	{
		float w = ofRandom(10, 30);
		float h = ofRandom(10, 30);
		boxes.push_back(std::make_shared<ofxBox2dRect>());
		boxes.back()->setPhysics(3.0, 0.53, 0.1);
		boxes.back()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
		boxes.back()->setVelocity(-scrollVelocity, 0);
	}

	if (key == 'q')
	{
		auto tri = std::make_shared<ofxBox2dPolygon>();
		tri->addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		tri->setPhysics(100.0, 0.7, 1.0);
		tri->create(box2d.getWorld());

		triangles.push_back(tri);
		tri->setVelocity(-scrollVelocity, 0);
	}

	if (key == '-')
	{
		players.push_back(shared_ptr<Player>(new Player(mouseX,mouseY)));
		Player* player = players.back().get();
		player->create(box2d.getWorld());
	}

	if (key == '1')
	{
		resetGame();
	}

	if (key == 'p')
	{
		auto tri = std::make_shared<ofxBox2dPolygon>();
		tri->addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		tri->setPhysics(1000.0, 0.7, 1.0);
		tri->create(box2d.getWorld());
		tri->addForce(ofVec2f(0, 1), 50);
		triangles.push_back(tri);
	}

	if (key == 'm')
	{
		float r = 20;		// a random radius 4px - 20px
		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(3.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), mouseX, mouseY, r);
		circles.back()->setVelocity(5.0, 0.0);
	}

	if (key == 'w')
	{
		triangles[0]->setVelocity(0, -5);
	}

	if (key == 'a')
	{
		triangles[0]->setVelocity(-5, 0);
	}

	if (key == 's')
	{
		triangles[0]->setVelocity(0, 5);
	}

	if (key == 'd')
	{
		triangles[0]->setVelocity(5, 0);
	}

	if (key == ' ') //Follow mouse with constant velocity (currently set at 5)
	{
		followMouse = !followMouse;
	}

	if (key == '=') //Create a planet with gravity and varying size
	{
		float r = ofRandom(40, 70);		// a random radius 4px - 20px
		planets.push_back(std::make_shared<ofxBox2dCircle>());
		planets.back()->setPhysics(100000.0, 0, 0.1);
		planets.back()->setup(box2d.getWorld(), mouseX, mouseY, r);

	}

	if (key == 'z')
	{
		shootCircle();
	}

	if (key == 'f') bMouseForce = !bMouseForce;
	if (key == 't') ofToggleFullscreen();
}

//Shoots circle by placing it in front of player with an initial forward velocity
void ofApp::shootCircle()
{
	auto player = players[0];

	ofVec2f playerPos = player->getPosition();
	int playerX = playerPos.x;
	int playerY = playerPos.y;

	float r = 10;		// a random radius 4px - 20px
	shotCircles.push_back(std::make_shared<ofxBox2dCircle>());
	shotCircles.back()->setPhysics(3.0, 0.53, 0.1);
	shotCircles.back()->setup(box2d.getWorld(), playerX + r + 22 , playerY , r);
	shotCircles.back()->setVelocity(60.0, 0.0);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	//drawing.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	mouseDown = true;

	std::cout << "Mouse is being pressed\n";
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	mouseDown = false;
}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e)
{
	if (e.a != NULL && e.b != NULL)
	{
		if (e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_circle)
		{
			auto player = players[0];
			std::cout << "Contact made with player and circle\n";
			player->takeDamage(5);
			std::cout << "Player health: " << player->currentHealth() << "\n";
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
			std::cout << "Contact made with player and circle\n";
		}
	}
}

void ofApp::resetGame()
{
	std::cout << "Entering reset game\n";

	auto world = box2d.getWorld();

	circles.erase(circles.begin(), circles.end());
	planets.erase(planets.begin(), planets.end());
	players.erase(players.begin(), players.end());
	createPlayer();
	boosters.erase(boosters.begin(), boosters.end());
	createPlayerBoosters();
	shotCircles.erase(shotCircles.begin(), shotCircles.end());
}