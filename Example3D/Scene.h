#pragma once
#include "glm\glm\glm.hpp"
#include "Camera.h"
#include "Instance.h"
#include <vector>


class MyApplication;

class Scene
{
public:
	Scene();
	~Scene();

	void draw();

	void useShader(unsigned int shader);

	glm::mat4 getCameraMatrix();

	std::vector<Instance*> m_instances;

	Camera m_camera;

	glm::vec3 m_lightDir;

	glm::vec3 m_pointLights[4];

	glm::vec3 m_pointLightColours[4];

	float m_pointLightPowers[4];

	MyApplication* app;
};

