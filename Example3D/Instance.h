#pragma once
#include "glm\glm\glm.hpp"
#include "Model.h"
#include "Shader.h"

class Scene;

class Instance
{
public:
	Instance();
	~Instance();
	
	//Instance(Model* model);

	Instance(Model* model, Shader* shader, glm::vec3 pos = glm::vec3(0), glm::vec3 euler = glm::vec3(0), glm::vec3 scale = glm::vec3(1.0f));

	void updateTransform();

	void draw(Scene* scene);
	
	glm::vec3 m_pos;
	glm::vec3 m_euler;
	glm::vec3 m_scale;

	glm::mat4 m_transform;

	Model* m_model;

	Shader* m_shader;
};

