#pragma once
#include "glm\glm\glm.hpp"
#include "Camera.h"
#include "Instance.h"
#include "Shader.h"
#include <vector>


class Application;

class Scene
{
public:
	Scene();
	~Scene();

	void draw();

	void useShader(Shader* shader);

	void update(float dt, float screenHeight, float screenWidth, Camera & camera, bool orbitingLight);

	glm::mat4 getCameraMatrix();

	std::vector<Instance*> m_instances;

	Camera m_camera;

	Shader* m_shader;

	glm::vec3 m_lightDir;

	glm::vec3 m_pointLights[4];

	glm::vec3 m_pointLightColours[4];

	glm::vec3 m_lightColour;

	float m_pointLightPowers[4];

	float m_specPow;

	float m_lightIntensity;

	float m_screenHeight;

	float m_screenWidth;

	static int culled;

	bool showBounds;
};

