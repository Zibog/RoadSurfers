#include <iostream>

using namespace std;

extern float offset[3];
extern float rotateGlob[3];
extern float lightPos[3];


void decAxis(int axis)//0x 1y 2z
{
    offset[axis] -= 0.01;
    cout << "inc " << axis << endl;
    cout << offset[0] << " " << offset[1] << " " << offset[2] << endl;
}
void incAxis(int axis)//0x 1y 2z
{
    offset[axis] += 0.01;
    cout << "dec " << axis << endl;
    cout << offset[0] << " " << offset[1] << " " << offset[2] << endl;
}
void decRotateAxis(int axis)//0x 1y 2z
{
    rotateGlob[axis] += 0.01;
    cout << rotateGlob[0] << " " << rotateGlob[1] << " " << rotateGlob[2] << endl;
}
void incRotateAxis(int axis)//0x 1y 2z
{
    rotateGlob[axis] -= 0.01;
    cout << rotateGlob[0] << " " << rotateGlob[1] << " " << rotateGlob[2] << endl;
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

