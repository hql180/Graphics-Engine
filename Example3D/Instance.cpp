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
	scene->useShader(m_model->isAnimated() ? m_animatedShader : m_shader);

	m_model->Draw(m_transform, )
}
