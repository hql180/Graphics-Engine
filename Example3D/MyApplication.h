#pragma once
#include "Application.h"
#include "Camera.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\mat4x4.hpp"
#include "tiny_obj_loader.h"
#include "Model.h"

class MyApplication : public aie::Application
{
public:
	MyApplication();
	virtual ~MyApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float dt);
	virtual void draw();

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	};	

	//void generateGrid(unsigned int rows, unsigned int cols); // function to create grid



	Camera camera;



private:
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_modelMatrix;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	const char* vsSource = "../Example3D/Shaders/vertexShader.vert";
	const char* fsSource = "../Example3D/Shaders/fragmentShader.frag";

	/*const char* vsSource = "#version 410\n \ layout(location=0) in vec4 position;\ 	layout(location=1) in vec4 colour; \ out vec4 vColour; \ uniform mat4 projectionViewWorldMatrix; \ void main() { vColour = colour; gl_Position = projectionViewWorldMatrix * position; }";
	const char* fsSource = "#version 410\n \ in vec4 vColour; \ out vec4 fragColor; \ void main() { fragColor = vColour; }";
*/
	float m_time;

	unsigned int m_programID;

	unsigned int m_animProgramID;

	glm::vec3 lightDirection;
	glm::vec3 lightColour;

	float specPow;	

	Model testModel;
};

