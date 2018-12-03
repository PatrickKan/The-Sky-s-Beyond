#include "ofApp.h"

static int pts[] = { 257,219,257,258,259,274,263,325,266,345,266,352,269,369,276,387,286,415,291,425,302,451,308,462,316,472,321,480,328,488,333,495,339,501,345,505,350,507,365,515,370,519,377,522,382,525,388,527,405,534,426,538,439,539,452,539,468,540,485,540,496,541,607,541,618,539,625,537,641,530,666,513,682,500,710,476,723,463,727,457,729,453,732,450,734,447,738,440,746,423,756,404,772,363,779,343,781,339,784,327,789,301,792,278,794,267,794,257,795,250,795,232,796,222,796,197,797,195,797,188,796,188 };
static int nPts = 61 * 2;

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(true);
	//ofBackgroundHex(0xfdefc2);
	ofBackground(ofColor(255, 255, 255));
	ofSetLogLevel(OF_LOG_NOTICE);

	background.load("C:\\Users\\Patrick Kan\\source\\repos\\finalproject-PatrickKan\\TheSkysBeyond\\TheSkysBeyond\\images\\space_background2.png");

	view.setPosition(ofPoint(mouseX, mouseY, 0));

	bMouseForce = false;
	followMouse = false;

	box2d.init();
	box2d.setGravity(0, 0);
	box2d.createGround();
	box2d.setFPS(60.0);
	box2d.registerGrabbing();

	// lets add a contour to start
	for (int i = 0; i < nPts; i += 2)
	{
		float x = pts[i];
		float y = pts[i + 1];
		edgeLine.addVertex(x, y);
	}

	// make the shape
	edgeLine.setPhysics(0.0, 0.5, 0.5);
	edgeLine.create(box2d.getWorld());
}

//--------------------------------------------------------------
void ofApp::update()
{

	box2d.update();

	view.move(mouseX, mouseY, 0);

	//For planet force, gravity should be equivalent to 1/r^2, but not too fast when approaching zero (check w/ if for r > 0.01)

	if (bMouseForce)
	{
		float strength = 8.0;
		float damping = 1.0f;
		float minDis = 100;
		for (auto i = 0; i < circles.size(); i++)
		{
			circles[i]->addAttractionPoint(mouseX, mouseY, strength);
			circles[i]->setDamping(damping, damping);
		}
		for (auto i = 0; i < customParticles.size(); i++)
		{
			customParticles[i]->addAttractionPoint(mouseX, mouseY, strength);
			customParticles[i]->setDamping(damping, damping);
		}
		for (auto i = 0; i < boxes.size(); i++)
		{
			boxes[i]->addAttractionPoint(mouseX, mouseY, strength);
			boxes[i]->setDamping(damping, damping);
		}
		for (auto i = 0; i < triangles.size(); i++)
		{
			triangles[i]->addAttractionPoint(mouseX, mouseY, strength);
			triangles[i]->setDamping(damping, damping);
		}
	}

	if (followMouse && triangles[0])
	{
		int velocity = 5;
		ofVec2f curr_pos = triangles[0]->getPosition();
		int x_pos = curr_pos.x;
		int y_pos = curr_pos.y;

		float angle = atan2(y_pos - mouseY, mouseX - x_pos);
		triangles[0]->setVelocity(velocity*cos(angle), -velocity * sin(angle));
	}

	//Planet gravity
	for (auto planet : planets)
	{
		float gravity = 0;

		ofVec2f planet_pos = planet->getPosition();
		int planet_rad = planet->getRadius();
		int planet_x = planet_pos.x;
		int planet_y = planet_pos.y;

		for (auto circle : circles)
		{
			gravity = ComputeGravity(circle->getPosition(), planet_pos, planet_rad);
			circle->addAttractionPoint(planet_x, planet_y, gravity);
		}
		for (auto particle : customParticles)
		{
			gravity = ComputeGravity(particle->getPosition(), planet_pos, planet_rad);
			particle->addAttractionPoint(planet_x, planet_y, gravity);
		}
		for (auto box : boxes)
		{
			gravity = ComputeGravity(box->getPosition(), planet_pos, planet_rad);
			box->addAttractionPoint(planet_x, planet_y, gravity);
		}
		for (auto triangle : triangles)
		{
			gravity = ComputeGravity(triangle->getPosition(), planet_pos, planet_rad);
			triangle->addAttractionPoint(planet_x, planet_y, gravity);
		}
	}


	// remove shapes offscreen
	ofRemove(boxes, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(customParticles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(triangles, ofxBox2dBaseShape::shouldRemoveOffScreen);
	ofRemove(planets, ofxBox2dBaseShape::shouldRemoveOffScreen);
}

float ofApp::ComputeGravity(ofVec2f curr_pos, ofVec2f planet_pos, int planet_rad)
{
	int x_pos = curr_pos.x;
	int y_pos = curr_pos.y;

	int planet_x = planet_pos.x;
	int planet_y = planet_pos.y;

	//Hypot taken from https://en.cppreference.com/w/cpp/numeric/math/hypot
	float distance = std::hypot(x_pos - planet_x, y_pos - planet_y);

	float gravity = 100 * float(planet_rad) / pow(distance, 2);

	return gravity;
}


//--------------------------------------------------------------
void ofApp::draw()
{

	background.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	for (auto i = 0; i < circles.size(); i++)
	{
		ofFill();
		ofSetHexColor(0x90d4e3);
		circles[i]->draw();
	}

	for (auto i = 0; i < boxes.size(); i++)
	{
		ofFill();
		ofSetHexColor(0xe63b8b);
		boxes[i]->draw();
	}

	for (auto i = 0; i < customParticles.size(); i++)
	{
		customParticles[i]->draw();
	}

	for (int i = 0; i < triangles.size(); i++)
	{
		ofFill();
		ofSetHexColor(0xEE82EE);
		triangles[i]->draw();
	}

	for (auto planet : planets)
	{
		ofFill();
		ofSetHexColor(0xcc3333);
		planet->draw();
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
	info += "Press [s] to draw a line strip [" + ofToString(bDrawLines) + "]\n";
	info += "Press [f] to toggle Mouse Force [" + ofToString(bMouseForce) + "]\n";
	info += "Press [c] for circles\n";
	info += "Press [b] for blocks\n";
	info += "Press [z] for custom particle\n";
	info += "Press [q] for triangles\n";
	info += "Press [=] to create a planet with gravity\n";
	info += "Press [0] to have the first triangle follow the mouse\n";
	info += "Use wasd controls to move first triangle\n";
	info += "MouseX: " + std::to_string(mouseX) + "\n";
	info += "MouseY: " + std::to_string(mouseY) + "\n";
	info += "Total Bodies: " + ofToString(box2d.getBodyCount()) + "\n";
	info += "Total Joints: " + ofToString(box2d.getJointCount()) + "\n\n";
	info += "FPS: " + ofToString(ofGetFrameRate()) + "\n";
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
	}

	if (key == 'b')
	{
		float w = ofRandom(4, 20);
		float h = ofRandom(4, 20);
		boxes.push_back(std::make_shared<ofxBox2dRect>());
		boxes.back()->setPhysics(3.0, 0.53, 0.1);
		boxes.back()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
	}

	if (key == '1')
	{
		float r = ofRandom(30, 50);		// a random radius 4px - 20px
		circles.push_back(std::make_shared<ofxBox2dCircle>());
		circles.back()->setPhysics(0.0, 0.53, 0.1);
		circles.back()->setup(box2d.getWorld(), mouseX, mouseY, r);
	}

	if (key == 'z')
	{
		customParticles.push_back(std::make_shared<CustomParticle>());
		CustomParticle * p = customParticles.back().get();
		float r = ofRandom(3, 10);		// a random radius 4px - 20px
		p->setPhysics(0.4, 0.53, 0.31);
		p->setup(box2d.getWorld(), mouseX, mouseY, r);
		p->color.r = ofRandom(20, 100);
		p->color.g = 0;
		p->color.b = ofRandom(150, 255);
	}

	if (key == 'q')
	{
		auto tri = std::make_shared<ofxBox2dPolygon>();
		tri->addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		tri->setPhysics(1.0, 0.7, 1.0);
		tri->create(box2d.getWorld());

		triangles.push_back(tri);
	}

	if (key == 'p')
	{
		auto tri = std::make_shared<ofxBox2dPolygon>();
		tri->addTriangle(ofPoint(mouseX - 10, mouseY), ofPoint(mouseX, mouseY - 10), ofPoint(mouseX + 10, mouseY));
		tri->setPhysics(1.0, 0.7, 1.0);
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

	if (key == '0') //Follow mouse with constant velocity
	{
		followMouse = !followMouse;

		//cout << "\nAngle: " << to_string(angle) << "\n x pos: " << x_pos << "\ny_pos:" << y_pos << "\n";
	}

	if (key == '=') //Create a planet with gravity and varying size
	{
		float r = ofRandom(40, 70);		// a random radius 4px - 20px
		planets.push_back(std::make_shared<ofxBox2dCircle>());
		planets.back()->setPhysics(10000.0, 0, 0.1);
		planets.back()->setup(box2d.getWorld(), mouseX, mouseY, r);

	}

	if (key == 'f') bMouseForce = !bMouseForce;
	if (key == 't') ofToggleFullscreen();
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	drawing.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

	if (edgeLine.isBody())
	{
		drawing.clear();
		edgeLine.destroy();
	}

	drawing.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

	drawing.setClosed(false);
	drawing.simplify();

	edgeLine.addVertexes(drawing);
	//polyLine.simplifyToMaxVerts(); // this is based on the max box2d verts
	edgeLine.setPhysics(0.0, 0.5, 0.5);
	edgeLine.create(box2d.getWorld());

	drawing.clear();
}