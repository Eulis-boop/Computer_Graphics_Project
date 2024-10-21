#pragma once
class Vertex
{
public:
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;
	//Colores RGB
	float r, g, b;
	Vertex(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 1.0f)
		: x(x_), y(y_), z(z_), w(w_) {}
};

