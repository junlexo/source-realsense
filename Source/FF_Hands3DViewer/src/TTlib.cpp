// 2016  05 30

#include "TTlib.h"


// --------------------------------------------------------------------------//
// --------------------------------------------------------------------------//



//===========================================================================//

void HandDetail::runOffStream(string fileName)
{
	freopen(fileName.c_str(), "r", stdin);

	while (true)
	{
		Sleep(10);

		inFullHand();

		updateHandShape();
		updateHandGesture();

		// output
		if (getHandGesture() == HandDetail::PULL_LEFT)
			cout << "OK" << endl;
	}
}


//===========================================================================//

HandDetail::HandDetail()
{
	initializePositionWorld(&preMid0);
	initializePositionWorld(&preIndexTip);

	i_handShape = HandDetail::NONES;
	straightFingers = "";
	changeShape = false;
	
	i_handGesture = HandDetail::NONEG;
	previousGesture = new char[NUM_OF_HAND_PRE_MOVEMENT];
	resetPreviousGesture();
	isMouseOn = false;

	MouseSetup(&mouseInput);

	frameCount = 0;
	gestureCount = 0;
	mouseCount = 0;
	gestureDelay = false;
	mouseDelayLeft = false;
}


//===========================================================================//

bool HandDetail::frameGate()
{
	frameCount++;

	// pass n-1 Frame
	if (frameCount < FRAME_SPACING)
		return false;

	frameCount = 0;
	return true;
}


//===========================================================================//

bool HandDetail::gestureGate()
{
	gestureCount++;

	// pass n Calls
	if (gestureCount <= GESTURE_SPACING)
		return false;

	gestureCount = 0;
	return true;
}


//===========================================================================//

bool HandDetail::mouseGate()
{
	mouseCount++;

	// pass n Calls
	if (mouseCount <= MOUSE_SPACING)
		return false;

	mouseCount = 0;
	return true;
}


//===========================================================================//

void HandDetail::updateHandDetail(Node<PXCHandData::JointData>* rootDataNode)
{	
	if (!frameGate())
	{
		changeShape = false;
		return;
	}

	// save previous Values
	preMid0 = mid0; 

	if (isMouseOn)  // MouseOn -> save the previous Index Tip Data
		preIndexTip = indexTip;

	// get New Fingers Values
	wrist = rootDataNode->getNodeValue();
	
	thumb0 = rootDataNode->getChildNodes()[0].getNodeValue();
	thumb1 = rootDataNode->getChildNodes()[0].getChildNodes()[0].getNodeValue();
	thumb2 = rootDataNode->getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	thumbTip = rootDataNode->getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	index0 = rootDataNode->getChildNodes()[1].getNodeValue();
	index1 = rootDataNode->getChildNodes()[1].getChildNodes()[0].getNodeValue();
	index2 = rootDataNode->getChildNodes()[1].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	indexTip = rootDataNode->getChildNodes()[1].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	mid0 = rootDataNode->getChildNodes()[2].getNodeValue();
	mid1 = rootDataNode->getChildNodes()[2].getChildNodes()[0].getNodeValue();
	mid2 = rootDataNode->getChildNodes()[2].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	midTip = rootDataNode->getChildNodes()[2].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	ring0 = rootDataNode->getChildNodes()[3].getNodeValue();
	ring1 = rootDataNode->getChildNodes()[3].getChildNodes()[0].getNodeValue();
	ring2 = rootDataNode->getChildNodes()[3].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	ringTip = rootDataNode->getChildNodes()[3].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	pinky0 = rootDataNode->getChildNodes()[4].getNodeValue();
	pinky1 = rootDataNode->getChildNodes()[4].getChildNodes()[0].getNodeValue();
	pinky2 = rootDataNode->getChildNodes()[4].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	pinkyTip = rootDataNode->getChildNodes()[4].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	updateHandShape();

	// if Mouse Mode OFF -> continue detect new Gesture
	if (isMouseOn == false)
		updateHandGesture();
	else if (changeShape == false)  // Mouse Mode ON -> process MouseMove
		updateCursor();
}


//===========================================================================//

void HandDetail::updateHandShape()
{
	int dem = 0; // count the number of straight Fingers

	straightFingers = ""; // which Finger is straight

	// straight Thumb
	if (isStraightFinger(&thumb0, &thumb1, &thumb2, &thumbTip))
	{
		dem++;
		straightFingers += "thumb ";
	}

	// straight Index
	if (isStraightFinger(&index0, &index1, &index2, &indexTip) && isStraightFingerH(&wrist, &index0, &indexTip))
	{
		dem++;
		straightFingers += "index ";
	}
		
	// straight Mid
	if (isStraightFinger(&mid0, &mid1, &mid2, &midTip) && isStraightFingerH(&wrist, &mid0, &midTip))
	{
		dem++;
		straightFingers += "mid ";
	}
		
	// straight Ring
	if (isStraightFinger(&ring0, &ring1, &ring2, &ringTip) && isStraightFingerH(&wrist, &ring0, &ringTip))
	{
		dem++;
		straightFingers += "ring ";
	}
		
	// straight Pinky
	if (isStraightFinger(&pinky0, &pinky1, &pinky2, &pinkyTip) && isStraightFingerH(&wrist, &pinky0, &pinkyTip))
	{
		dem++;
		straightFingers += "pinky ";
	}
			
	HandDetail::HandShape handShape;

	switch (dem)
	{
	case 0:
		handShape = HandDetail::ZERO;
		break;
	case 1:
		handShape = HandDetail::ONE;
		break;
	case 2:
		handShape = HandDetail::TWO;
		break;
	case 3:
		handShape = HandDetail::THREE;
		break;
	case 4:
		handShape = HandDetail::FOUR;
		break;
	case 5:
		handShape = HandDetail::FIVE;
		break;
	default:
		handShape = HandDetail::NONES;
		break;
	}

	// Special Shape -> Mouse Mode On
	if (straightFingers == "index " || straightFingers == "thumb index ")
	{
		handShape = HandDetail::MOUSE;
		isMouseOn = true;
	}
	else
		isMouseOn = false;

	// detect Difference
	if (i_handShape == handShape)
		changeShape = false;

	else // i_handShape != handShape
	{
		i_handShape = handShape;
		changeShape = true;
	}
}


//===========================================================================//

void HandDetail::updateHandGesture()
{
	updateGesture();  // push current state Movement into the Queue. Up, Right, Down or Left

	// initialize
	int up, right, down, left;
	left = right = up = down = 0;

	// count in the Queue
	for (int i = 0; i < NUM_OF_HAND_PRE_MOVEMENT; i++)
	{
		switch (previousGesture[i])
		{
		case 'U':
			up++;
			break;
		case 'R':
			right++;
			break;
		case 'D':
			down++;
			break;
		case 'L':
			left++;
			break;
		default:
			break;
		}
	}

	// get current HAND GESTURE
	if (up >= int(NUM_OF_HAND_PRE_MOVEMENT * RATE_OF_HAND_GESTURE_DETECTOR))
		i_handGesture = HandDetail::PULL_UP;

	else if (right >= int(NUM_OF_HAND_PRE_MOVEMENT * RATE_OF_HAND_GESTURE_DETECTOR))
		i_handGesture = HandDetail::PULL_RIGHT;

	else if (down >= int(NUM_OF_HAND_PRE_MOVEMENT * RATE_OF_HAND_GESTURE_DETECTOR))
		i_handGesture = HandDetail::PULL_DOWN;

	else if (left >= int(NUM_OF_HAND_PRE_MOVEMENT * RATE_OF_HAND_GESTURE_DETECTOR))
		i_handGesture = HandDetail::PULL_LEFT;

	else
		i_handGesture = HandDetail::NONEG;

	// Reset Queue
	if (i_handGesture != HandDetail::NONEG)
	{
		resetPreviousGesture();
	}
}


//===========================================================================//

void HandDetail::updateGesture() // need to be improved
{
	float deltaX = mid0.positionWorld.x - preMid0.positionWorld.x;
	float deltaY = mid0.positionWorld.y - preMid0.positionWorld.y;


	if (abs(deltaX) >= abs(deltaY))
	{
		if (deltaX >= LEAST_MOVE_DISTANCE)  // left - increase X
			pushGesture('L');
		else if (deltaX <= -LEAST_MOVE_DISTANCE)  // right - decrease X
			pushGesture('R');
		else
			pushGesture('N');
	}
	else
	{
		if (deltaY >= LEAST_MOVE_DISTANCE)  // up - increase Y
			pushGesture('U');
		else if (deltaY <= -LEAST_MOVE_DISTANCE)  // down - decrease Y
			pushGesture('D');
		else
			pushGesture('N');
	}
}


//===========================================================================//

void HandDetail::pushGesture(char gestureHeading)
{
	for (int i = NUM_OF_HAND_PRE_MOVEMENT - 1; i >= 1; i--)
	{
		previousGesture[i] = previousGesture[i-1];
	}

	previousGesture[0] = gestureHeading;
}


//===========================================================================//

void HandDetail::resetPreviousGesture()
{
	for (int i = 0; i < NUM_OF_HAND_PRE_MOVEMENT; i++)
		previousGesture[i] = 'N';
}


//===========================================================================//

void HandDetail::updateCursor()
{
	// get distance of Hand Move
	float deltaX = mid0.positionWorld.x - preMid0.positionWorld.x;
	float deltaY = mid0.positionWorld.y - preMid0.positionWorld.y;

	// Mouse Click Event is Delayed
	if (mouseDelayLeft)
	{
		if (mouseGate())
			mouseDelayLeft = false;
	}
	else  // mouseDelayLeft == false
	{
		// detect MouseClick
		float deltaIndexTip = abs(indexTip.positionWorld.x - preIndexTip.positionWorld.x)
			+ abs(indexTip.positionWorld.y - preIndexTip.positionWorld.y) + abs(indexTip.positionWorld.z - preIndexTip.positionWorld.z);
		
		float deltaZ = abs(indexTip.positionWorld.z - preIndexTip.positionWorld.z);

		//if (deltaIndexTip >= abs(deltaX) * MOUSE_CLICK_RATE &&
		//	deltaIndexTip >= abs(deltaY) * MOUSE_CLICK_RATE && deltaIndexTip >= LEAST_MOVE_DISTANCE*3)
		if (deltaZ >= LEAST_MOVE_DISTANCE)
		{
			// process Mouse Click Event
			//MouseClick(&mouseInput);
			mouseDelayLeft = true;
		}
	}


	// compute the distance that Mouse will move (pixel)
	int x = (int) (-deltaX / MOVE_DISTANCE_PER_PIXCEL);
	int y = (int) (-deltaY / MOVE_DISTANCE_PER_PIXCEL);

	// Thie out
	//cout << x << " " << y << endl;

	if (x != 0 || y != 0)
		MouseMove(&mouseInput, x, y);
}


//===========================================================================//

bool HandDetail::getChangeShape()
{
	return changeShape;
}


//===========================================================================//

string HandDetail::getStraightFingers()
{
	return straightFingers;
}


//===========================================================================//

string HandDetail::getHandShapeToString()
{
	switch (i_handShape)
	{
	case HandDetail::NONES:
		return "NONES";
		break;
	case HandDetail::ZERO:
		return "ZERO";
		break;
	case HandDetail::ONE:
		return "ONE";
		break;
	case HandDetail::TWO:
		return "TWO";
		break;
	case HandDetail::THREE:
		return "THREE";
		break;
	case HandDetail::FOUR:
		return "FOUR";
		break;
	case HandDetail::FIVE:
		return "FIVE";
		break;
	case HandDetail::MOUSE:
		return "MOUSE";
		break;
	default:
		return "NONE _ ERROR";
		break;
	}
}


//===========================================================================//

HandDetail::HandShape HandDetail::getHandShape()
{
	return i_handShape;
}


//===========================================================================//

string HandDetail::getHandGestureToString()
{
	switch (i_handGesture)
	{
	case HandDetail::NONEG:
		return "NONE";
		break;
	case HandDetail::PULL_UP:
		return "UP";
		break;
	case HandDetail::PULL_RIGHT:
		return "RIGHT";
		break;
	case HandDetail::PULL_DOWN:
		return "DOWN";
		break;
	case HandDetail::PULL_LEFT:
		return "LEFT";
		break;

	default: 
		return "NONE _ ERROR";
		break;
	}
}


//===========================================================================//

HandDetail::HandGesture HandDetail::getHandGesture()
{
	if (gestureDelay)  // can not get New Hand Gesture
	{
		if (gestureGate())
			gestureDelay = false;   // can get New Hand Gesture
	
		return HandDetail::NONEG;
	}

	// gestureDelay == false
	if (i_handGesture != HandDetail::NONEG)
		gestureDelay = true;

	HandGesture hG = i_handGesture;
	i_handGesture = HandDetail::NONEG;

	return hG;
}


//===========================================================================//

void HandDetail::inFullHand()
{
	// save previous mid0 value
	preMid0 = mid0;  
	preIndexTip = indexTip;

	// cin >> all Joint of Hand
	inJoint(wrist);
	inJoint(thumb0); inJoint(thumb1);  inJoint(thumb2); inJoint(thumbTip);
	inJoint(index0); inJoint(index1); inJoint(index2); inJoint(indexTip);
	inJoint(mid0); inJoint(mid1); inJoint(mid2); inJoint(midTip);
	inJoint(ring0); inJoint(ring1); inJoint(ring2); inJoint(ringTip);
	inJoint(pinky0); inJoint(pinky1); inJoint(pinky2); inJoint(pinkyTip);
}
