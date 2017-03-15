#include "Camera.h"
#include "Application.h"
#include "glm\glm\ext.hpp"
#include "Input.h"

const float deg2Rad = 3.14159f / 180.0f;


Camera::~Camera()
{
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, 
							w / h, 
							0.1f, 1000.f);
}

glm::mat4 Camera::GetViewMatrix()
{
	float thetaR = theta * deg2Rad;
	float phiR = phi * deg2Rad;
	glm::vec3 forward(cos(phiR)*cos(thetaR), sin(phiR), cos(phiR)*sin(thetaR));
	return glm::lookAt(position, position + forward, glm::vec3(0, 1, 0));
}

void Camera::Update()
{
	aie::Input* input = aie::Input::getInstance();
	float thetaR = theta * deg2Rad;
	float phiR = phi * deg2Rad;

	//calculate the forwards and right axes and up axis for the camera
	glm::vec3 forward(cos(phiR)*cos(thetaR), sin(phiR), cos(phiR)*sin(thetaR));
	glm::vec3 right(-sin(thetaR), 0, cos(thetaR));
	glm::vec3 up(0, 1, 0);

	float deltaTime = 0.1f;

	float mx = (float)input->getMouseX();
	float my = (float)input->getMouseY();


	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		theta += 0.1f * (mx - lastMouseX);
		phi += 0.05f * (my - lastMouseY);
	}

	lastMouseX = (int)mx;
	lastMouseY = (int)my;

	// use WASD, ZX keys to move camera around
	if (input->isKeyDown(aie::INPUT_KEY_Q))
		position += -up * deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_E))
		position += up*deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_A))
		position += -right* deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_D))
		position += right* deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_W))
		position += forward* deltaTime;

	if (input->isKeyDown(aie::INPUT_KEY_S))
		position += -forward* deltaTime;
}
