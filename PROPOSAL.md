# Project Proposal

The Sky’s Beyond takes place in the expansive world of outer space. Within a triangular spaceship, you explore the hues of planets, the darkness of space, and the light of stars. It utilizes miniature sized planets alongside traditional physics, which would cause a spaceship to become pulled by the gravity of planets. However, one must be careful not to be pulled into a planet’s grasp -- causing a inevitable crash and end to the adventure.

## Libraries included

In order to build the application, I would utilize the ofxBox2d library ( https://github.com/vanderlin/ofxBox2d), which creates circles with physical properties to represent asteroids. I would modify the physical circles to make the user take damage upon impact of an asteroid. 

Moreover, in order to integrate sounds into my project I would use the OSC library (Open Sound Control) alongside ofSoundPlayer in order to create effect and background sound to immerse the user into the game. 

For starting code, I will begin by utilizing the openFramework book’s guide on game design for an initial spaceship (https://openframeworks.cc/ofBook/chapters/game_design.html). However, I will initially modify the implementation to have smooth turning.

### Features

The user will be in a triangular spaceship, with movement will be guided by mouse movement. Thrusters can be engaged by pressing the spacebar or clicking. 

There are numerous levels within the game, each increasing in difficulty with more obstacles and challenges, whether that be larger planets (with a greater gravitational pull), less fuel (for thrusters), or more asteroids. 
In order to implement both planets and thrusters, I will use the acceleration handlers from https://openframeworks.cc/documentation/ofxAccelerometer/ofxAccelerometerHandler/ 

There will be bars for the user to track velocity, and the game will scroll based on the position of the horizontal position of the spaceship. Additionally, an initial loading screen will guide the user into level selection, where they can pick their desired stage.

Additionally, if there is time, then a possibility could be to implement a shaking screen upon destruction, portals, or moving enemies.
