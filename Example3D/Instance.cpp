#include "Instance.h"
#include "glm\glm\ext.hpp"
#include "Scene.h"
#include "gl_core_4_4.h"

Instance::Instance()
{
}


Instance::~Instance()
{
}

//Instance::Instance(Model * model)
//{
//	m_model = model;
//	m_pos = glm::vec3(0);
//	m_euler = glm::vec3(0);
//	m_scale = glm::vec3(1);
//}

Instance::Instance(Model * model, Shader* shader, glm::vec3 pos, glm::vec3 euler, glm::vec3 scale)
{
	m_model = model;
	m_pos = pos;
	m_euler = euler;
	m_scale = scale;
	m_shader = shader;
}

void Instance::updateTransform()
{
	float rad = 6.28f / 360.0f;
	m_transform = glm::translate(m_pos) *
		glm::rotate(m_euler.z * rad, glm::vec3(0, 0, 1)) *
		glm::rotate(m_euler.y * rad, glm::vec3(0, 1, 1)) *
		glm::rotate(m_euler.x * rad, glm::vec3(1, 0, 0)) *
		glm::scale(m_scale);
}

void Instance::draw(Scene * scene)
{
	scene->useShader(m_shader);

	m_model->Draw(m_transform, scene->getCameraMatrix(), m_shader);
}
