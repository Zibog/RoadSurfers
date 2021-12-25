#pragma once
#include <gl/glew.h>

struct BusState {
public:
	float offset[3] = { 0.02, -2.74, 0.9 };
	float rotateGlob[3] = { 0.0, 0.0 ,0.0 };
	float lightPos[3] = { -13.0f, 0.0f, 0.0f };
	GLboolean lightOnGlobal = true;
};