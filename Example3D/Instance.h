#pragma once
#include "glm\glm\glm.hpp"
#include "Model.h"

class Scene;

class Instance
{
public:
	Instance();
	~Instance();

	void updateTransform();

	void draw(Scene* scene);
	
	glm::vec3 m_pos;
	glm::vec3 m_euler;
	glm::vec3 m_scale;

	glm::mat4 m_transform;

	Model* m_model;

	unsigned int m_shader;
	unsigned int m_animatedShader;
};

