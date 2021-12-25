#include <iostream>

using namespace std;

extern float busShift[3];
extern float busRotate[3];
extern float lightPos[3];


void decAxis(int axis)//0x 1y 2z
{
    busShift[axis] -= 0.01;
    cout << "inc " << axis << endl;
    cout << busShift[0] << " " << busShift[1] << " " << busShift[2] << endl;
}
void incAxis(int axis)//0x 1y 2z
{
    busShift[axis] += 0.01;
    cout << "dec " << axis << endl;
    cout << busShift[0] << " " << busShift[1] << " " << busShift[2] << endl;
}
void decRotateAxis(int axis)//0x 1y 2z
{
    busRotate[axis] += 0.01;
    cout << busRotate[0] << " " << busRotate[1] << " " << busRotate[2] << endl;
}
void incRotateAxis(int axis)//0x 1y 2z
{
    busRotate[axis] -= 0.01;
    cout << busRotate[0] << " " << busRotate[1] << " " << busRotate[2] << endl;
}

void turnLeft() {
    incAxis(0);
    incRotateAxis(2);
}

void turnRight() {
    decAxis(0);
    decRotateAxis(2);
}

void decLightPos(int axis)//0x 1y 2z
{
    lightPos[axis] += 0.1;
    cout << lightPos[0] << " " << lightPos[1] << " " << lightPos[2] << endl;
}
void incLightPos(int axis)//0x 1y 2z
{
    lightPos[axis] -= 0.1;
    cout << lightPos[0] << " " << lightPos[1] << " " << lightPos[2] << endl;
}

int position = 0;
bool leftTurn = false;
bool leftAlign = false;
bool rightTurn = false;
bool rightAlign = false;

void makeMovement() {
    if (leftTurn)
    {
        if (position == 0)
        {
            if (busShift[0] < 0.3)
            {
                turnLeft();
            }
            else
            {
                position = -1;
                leftTurn = false;
                leftAlign = true;
            }
        }
        else if (position == 1)
        {
            if (busShift[0] < 0.0)
            {
                turnLeft();
            }
            else
            {
                position = 0;
                leftTurn = false;
                leftAlign = true;
            }
        }
    }
    if (rightTurn)
    {
        if (position == 0)
        {
            if (busShift[0] > -0.3)
            {
                turnRight();
            }
            else
            {
                position = 1;
                rightTurn = false;
                rightAlign = true;
            }
        }
        else if (position == -1)
        {
            if (busShift[0] > 0.0)
            {
                turnRight();
            }
            else
            {
                position = 0;
                rightTurn = false;
                rightAlign = true;
            }
        }
    }
    if (leftAlign)
    {
        if (busRotate[2] < 3.14) {
            decRotateAxis(2);
        }
        else
        {
            leftAlign = false;
        }
    }
    else  if (rightAlign)
    {
        if (busRotate[2] > 3.14) {
            incRotateAxis(2);
        }
        else
        {
            rightAlign = false;
        }
    }
}

void moveLeft() {
    if (position != -1) {
        leftTurn = true;
        rightTurn = false;
    }
}

void moveRight() {
    if (position != 1) {
        rightTurn = true;
        leftTurn = false;
    }
}