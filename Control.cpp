#include <iostream>
#include "Control.h"

using namespace std;

extern float busShift[3];
extern float busRotate[3];
extern float lightPos[3];


int position = 0;
bool onLeft = false;
bool alignLeft = false;
bool onRight = false;
bool alignRight = false;


void decreseShift(int axis)//0x 1y 2z
{
    busShift[axis] -= 0.01;
   
}
void increaseShift(int axis)//0x 1y 2z
{
    busShift[axis] += 0.01;
}
void decreaseRotate(int axis)//0x 1y 2z
{
    busRotate[axis] += 0.01;

}
void increaseRotate(int axis)//0x 1y 2z
{
    busRotate[axis] -= 0.01;
}

void makeMovement() {
    if (onLeft)
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
                onLeft = false;
                alignLeft = true;
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
                onLeft = false;
                alignLeft = true;
            }
        }
    }
    if (onRight)
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
                onRight = false;
                alignRight = true;
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
                onRight = false;
                alignRight = true;
            }
        }
    }
    if (alignLeft)
    {
        if (busRotate[2] < 3.14) {
            decreaseRotate(2);
        }
        else
        {
            alignLeft = false;
        }
    }
    else  if (alignRight)
    {
        if (busRotate[2] > 3.14) {
            increaseRotate(2);
        }
        else
        {
            alignRight = false;
        }
    }
}


void turnLeft() {
    increaseShift(0);
    increaseRotate(2);
}

void turnRight() {
    decreseShift(0);
    decreaseRotate(2);
}

void decreaseLightPosition(int axis)//0x 1y 2z
{
    lightPos[axis] += 0.1;
    
}
void increaseLightPosition(int axis)//0x 1y 2z
{
    lightPos[axis] -= 0.1;

}


void moveLeft() {
    if (position != -1) {
        onLeft = true;
        onRight = false;
    }
}

void moveRight() {
    if (position != 1) {
        onRight = true;
        onLeft = false;
    }
}