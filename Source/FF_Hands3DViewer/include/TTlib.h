// 2016  05 30
/**
* @brief
* @save the Real World Coordinates of the current Frame 
* @detect Hand Shape, Hand Gesture
* @control the Mouse Movement correspond to The Movement of Hand
*/

#ifndef TTLIB_H
#define TTLIB_H

#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>

#include "pxchanddata.h"
#include "Tree.h"

using namespace std;

#define JOINT PXCHandData::JointData
#define STRAIGHT_RATE_FING 0.85
#define STRAIGHT_RATE_HAND 0.80

#define FRAME_SPACING 2   // update HandDetail every that quantity Frames 
#define GESTURE_SPACING 30   // 60FPS -> ~ 500 ms, pass that quantity Times after a Hand Gesture was be transmitted
#define MOUSE_SPACING 30   // 

#define MOVE_DISTANCE_PER_PIXCEL 0.0001   // metric
#define MOUSE_CLICK_RATE 5   // IndexTip move distance / Mid0 move distance
#define LEAST_MOVE_DISTANCE 0.005   // metric, minimum distance to be detected a gesture

#define NUM_OF_HAND_PRE_MOVEMENT 4   // The amount of pre-Movement will be saved for Gesture Detecting
#define RATE_OF_HAND_GESTURE_DETECTOR 0.8   // How many same pre-Movement to be Detected a Gesture


/**
* class HandDetail hold all Joint Coordinates of Hand and its current Shape, Gesture at the last Frame
*/
class HandDetail
{
public:	
	enum HandShape // define types of Hand Shape
	{
		NONES = -1,
		ZERO,
		ONE,
		TWO,
		THREE,
		FOUR,
		FIVE, 
		MOUSE
	};
	
	enum HandGesture // define types of Hand Gesture
	{
		NONEG = 0,
		PULL_UP,
		PULL_RIGHT,
		PULL_DOWN,
		PULL_LEFT
	};

	// public Methods
	HandDetail();
	void updateHandDetail(Node<PXCHandData::JointData>* );
	void runOffStream(string );  // input from TXT file to get Coordinate of Joints, one line - one frame

	void updateHandShape();
	void updateHandGesture();
	void updateGesture();
	void pushGesture(char );
	void resetPreviousGesture();  // set values with 0
	void updateCursor();   // only isMouseOn == true
	
	bool getChangeShape();
	bool frameGate();
	bool gestureGate();
	bool mouseGate();
	
	string getHandShapeToString();
	HandDetail::HandShape getHandShape();
	string getHandGestureToString();
	HandDetail::HandGesture getHandGesture();   // use delayGesture variable every time that get a Hand Gesture
	string getStraightFingers();  

	// input from File
	void inFullHand();
	
	
private:
	// Joints Data
	JOINT wrist;
	JOINT thumb0, thumb1, thumb2, thumbTip;
	JOINT index0, index1, index2, indexTip;
	JOINT mid0, mid1, mid2, midTip;
	JOINT ring0, ring1, ring2, ringTip;		
	JOINT pinky0, pinky1, pinky2, pinkyTip;
	
	// Joints Data at previous Frame, use to compute the Move Distance 
	JOINT preMid0, preIndexTip;

	HandShape i_handShape;  // Shape at the last Frame
	string straightFingers;
	
	HandGesture i_handGesture;  // save the lastest Gesture
	char* previousGesture;

	INPUT mouseInput;

	// counter, flag
	int frameCount;
	int gestureCount;
	int mouseCount;
	bool isMouseOn;
	bool changeShape;
	bool gestureDelay;
	bool mouseDelayLeft;
};


//===========================================================================//
/**
* In-Out methods
* Calculate methods
*/

// set values = 0
void initializePositionWorld(JOINT* );

// output positonImage of a Joint (x, y)
void outCoor2D(PXCHandData::JointData* );

// output positionWorld of a Joint (x, y, z)
void outCoor3D(PXCHandData::JointData* );

// output Spped if a Joint - include ENDL
void outSpeed3D(PXCHandData::JointData* );

// output all Joints, one Frame - one Line
void outFullHand3D(Node<PXCHandData::JointData>* );

// input a Joint
void inJoint(JOINT& );

// get Distance of 2 Points
float distance (JOINT* , JOINT* );

// detect whether the Finger is traight or not
bool isStraightFinger (JOINT* , JOINT* , JOINT* , JOINT* );
bool isStraightFingerH (JOINT* , JOINT* , JOINT*);

// detect whether the Finger is curved or not
bool isCurvedFinger (JOINT* , JOINT* , JOINT* );


/**
* Window methods
*/
void GenerateKey(int, BOOL);
void MouseSetup(INPUT* );
void MouseMove(INPUT *, int, int);
void MouseClick(INPUT *);

#endif

