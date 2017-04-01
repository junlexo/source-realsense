// 2016  05 30

#include "TTlib.h"


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

//===========================================================================//
void initializePositionWorld(JOINT* j)
{
	j->positionWorld.x = j->positionWorld.y = j->positionWorld.z = 0;
}


//===========================================================================//

void outCoor2D(JOINT* j) 
{
	cout << (int)j->positionImage.x << " " << (int)j->positionImage.y << " ";
}


//===========================================================================//

void outCoor3D(JOINT* j) 
{
	cout << j->positionWorld.x << " " << j->positionWorld.y << " " << j->positionWorld.z << " ";
}


//===========================================================================//

// output all Joints, one Frame - one Line
void outFullHand3D(Node<PXCHandData::JointData>* rootDataNode)
{
	JOINT wrist = rootDataNode->getNodeValue();
	JOINT thumb0 = rootDataNode->getChildNodes()[0].getNodeValue();
	JOINT thumb1 = rootDataNode->getChildNodes()[0].getChildNodes()[0].getNodeValue();
	JOINT thumb2 = rootDataNode->getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	JOINT thumbTip = rootDataNode->getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	JOINT index0 = rootDataNode->getChildNodes()[1].getNodeValue();
	JOINT index1 = rootDataNode->getChildNodes()[1].getChildNodes()[0].getNodeValue();
	JOINT index2 = rootDataNode->getChildNodes()[1].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	JOINT indexTip = rootDataNode->getChildNodes()[1].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	JOINT mid0 = rootDataNode->getChildNodes()[2].getNodeValue();
	JOINT mid1 = rootDataNode->getChildNodes()[2].getChildNodes()[0].getNodeValue();
	JOINT mid2 = rootDataNode->getChildNodes()[2].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	JOINT midTip = rootDataNode->getChildNodes()[2].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	JOINT ring0 = rootDataNode->getChildNodes()[3].getNodeValue();
	JOINT ring1 = rootDataNode->getChildNodes()[3].getChildNodes()[0].getNodeValue();
	JOINT ring2 = rootDataNode->getChildNodes()[3].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	JOINT ringTip = rootDataNode->getChildNodes()[3].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();

	JOINT pinky0 = rootDataNode->getChildNodes()[4].getNodeValue();
	JOINT pinky1 = rootDataNode->getChildNodes()[4].getChildNodes()[0].getNodeValue();
	JOINT pinky2 = rootDataNode->getChildNodes()[4].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	JOINT pinkyTip = rootDataNode->getChildNodes()[4].getChildNodes()[0].getChildNodes()[0].getChildNodes()[0].getNodeValue();
	
	// output the Data
	outCoor3D(&wrist);
	outCoor3D(&thumb0); outCoor3D(&thumb1); outCoor3D(&thumb2); outCoor3D(&thumbTip);
	outCoor3D(&index0); outCoor3D(&index1); outCoor3D(&index2); outCoor3D(&indexTip);
	outCoor3D(&mid0); outCoor3D(&mid1); outCoor3D(&mid2); outCoor3D(&midTip);
	outCoor3D(&ring0); outCoor3D(&ring1); outCoor3D(&ring2); outCoor3D(&ringTip);
	outCoor3D(&pinky0); outCoor3D(&pinky1); outCoor3D(&pinky2); outCoor3D(&pinkyTip);
	cout << endl;
}


//===========================================================================//

// input positionWorld Data of the Joint in Parameter
void inJoint(JOINT& j)
{
	cin >> j.positionWorld.x >> j.positionWorld.y >> j.positionWorld.z;
}


//===========================================================================//

float distance (JOINT* j1, JOINT* j2)
{
	// get delta
	float dx = j1->positionWorld.x - j2->positionWorld.x;
	float dy = j1->positionWorld.y - j2->positionWorld.y;
	float dz = j1->positionWorld.z - j2->positionWorld.z;
	
	// calculate the distance
	return sqrt(dx*dx + dy*dy + dz*dz);
}


//===========================================================================//

bool isStraightFinger (JOINT* j0, JOINT* j1, JOINT* j2, JOINT* jTip)
{
	float total = distance(j0, j1) + distance(j1, j2) + distance(j2, jTip);
	float fing = distance(j0, jTip);
	
	if (fing >= total*STRAIGHT_RATE_FING)
		return true;
	
	return false;
}


//===========================================================================//

bool isStraightFingerH (JOINT* wrist, JOINT* j0, JOINT* jTip)
{
	float total = distance(wrist, j0) + distance(j0, jTip);
	float fingH = distance(wrist, jTip);
	
	if (fingH >= total*STRAIGHT_RATE_HAND)
		return true;

	return false;
}


//===========================================================================//

bool isCurvedFinger (JOINT* wrist, JOINT* j0, JOINT* jTip)
{
	if (distance(wrist, j0) >= distance(wrist, jTip))
		return true;

	return false;
}


//===========================================================================//

/* This is a function to simplify usage of sending keys */
void GenerateKey(int vk, BOOL bExtended) {

	KEYBDINPUT  kb = { 0 };
	INPUT       Input = { 0 };

	/* Generate a "key down" */
	if (bExtended) { kb.dwFlags = KEYEVENTF_EXTENDEDKEY; }
	kb.wVk = vk;
	Input.type = INPUT_KEYBOARD;
	Input.ki = kb;
	SendInput(1, &Input, sizeof(Input));

	/* Generate a "key up" */
	ZeroMemory(&kb, sizeof(KEYBDINPUT));
	ZeroMemory(&Input, sizeof(INPUT));
	kb.dwFlags = KEYEVENTF_KEYUP;
	if (bExtended) { kb.dwFlags |= KEYEVENTF_EXTENDEDKEY; }
	kb.wVk = vk;
	Input.type = INPUT_KEYBOARD;
	Input.ki = kb;
	SendInput(1, &Input, sizeof(Input));

	return;
}


//===========================================================================//

void MouseSetup(INPUT *buffer)
{
	buffer->type = INPUT_MOUSE;
	buffer->mi.dx = 0;  //(0 * (0xFFFF / (GetSystemMetrics(SM_CXSCREEN) - 1)));
	buffer->mi.dy = 0;  //(0 * (0xFFFF / (GetSystemMetrics(SM_CYSCREEN) - 1)));
	buffer->mi.mouseData = 0;
	buffer->mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	buffer->mi.time = 0;
	buffer->mi.dwExtraInfo = 0;
}


//===========================================================================//

void MouseMove(INPUT *buffer, int x, int y) // in pixel
{
	POINT curMouse;
	GetCursorPos(&curMouse);

	x += curMouse.x;
	y += curMouse.y;

	buffer->mi.dx = int ( x * ((float)0xFFFF / GetSystemMetrics(SM_CXSCREEN)) );
	buffer->mi.dy = int ( y * ((float)0xFFFF / GetSystemMetrics(SM_CYSCREEN)) );
	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

	SendInput(1, buffer, sizeof(INPUT));
}


//===========================================================================//

void MouseClick(INPUT *buffer)
{
	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
	SendInput(1, buffer, sizeof(INPUT));
	
	Sleep(10);

	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
	SendInput(1, buffer, sizeof(INPUT));
}

