#include "ofApp.h"
#include "ofMain.h"

//Uncomment #define to run
//#define CATCH_CONFIG_RUNNER
#ifdef CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "../SkyTests.h"

int main()
{
	int result = Catch::Session().run();

	system("pause");
	return result;
}

#else

//========================================================================
int main()
{
	ofSetupOpenGL(1768, 956, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}

#endif