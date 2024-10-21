#pragma once
//#include "Vertex.h"
#include "Face.h"
#include <Vector>

using namespace std;

class Object3D
{
public:
	vector<Vertex> vertices;
	vector<Face> faces;
	float t = 0.0f;
};

