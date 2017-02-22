#include "glm\glm\glm.hpp"


#pragma once
class Camera
{
public:
	Camera() : theta(0), phi(-20), position(-10, 4, 0) {}
	~Camera();

	glm::mat4 GetProjectionMatrix(float w, float h);

	glm::mat4 GetViewMatrix();

	glm::vec3 position;

	void Update();

private:
	float theta;
	float phi;
	

	int lastMouseX;
	int lastMouseY;
};

