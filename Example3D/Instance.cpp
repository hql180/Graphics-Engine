#include "Instance.h"
#include "glm\glm\ext.hpp"
#include "Scene.h"
#include "gl_core_4_4.h"
#include "Gizmos.h"
#include <algorithm>


Instance::Instance()
{
}


Instance::~Instance()
{
}

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
	glm::mat4 mvp = scene->getCameraMatrix() * m_transform;
	//glm::vec3 screenPos(mvp[3][0] / mvp[3][3], mvp[3][1] / mvp[3][3], mvp[3][2]);


	//float edge = 0.75f;
	//if (screenPos.x < -edge || screenPos.x > edge ||
	//	screenPos.y < -edge || screenPos.y > edge)
	//{
	//	scene->culled++;
	//	return;
	//}

	glm::vec4 corner;
	glm::vec4 screenPos[8];
	glm::vec3 screenMin(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 screenMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < 8; i++)
	{
		corner.x = (i & 1) ? m_model->boundsMin.x : m_model->boundsMax.x;
		corner.y = (i & 2) ? m_model->boundsMin.y : m_model->boundsMax.y;
		corner.z = (i & 4) ? m_model->boundsMin.z : m_model->boundsMax.z;
		corner.w = 1;
		screenPos[i] = mvp* corner;
		screenPos[i].x /= screenPos[i].w;
		screenPos[i].y /= screenPos[i].w;
		screenPos[i].z /= screenPos[i].w;
		screenMin.x = std::min(screenMin.x, screenPos[i].x);
		screenMin.y = std::min(screenMin.y, screenPos[i].y);
		screenMin.z = std::min(screenMin.z, screenPos[i].z);
		screenMax.x = std::max(screenMax.x, screenPos[i].x);
		screenMax.y = std::max(screenMax.y, screenPos[i].y);
		screenMax.z = std::max(screenMax.z, screenPos[i].z);
		
	}
	float edge = 1.0f;
	if (screenMax.x < -edge || screenMin.x > edge
		|| screenMax.y < -edge || screenMin.y > edge
		|| screenMax.z < -edge || screenMin.z > edge)
	{
		scene->culled++;
		return;
	}

	scene->useShader(m_shader);

	if (scene->showBounds)
	{
		glm::vec3 center = glm::vec3((m_model->boundsMin + m_model->boundsMax)*0.5f);
		aie::Gizmos::addAABB(center, (m_model->boundsMax - m_model->boundsMin)*0.5f, glm::vec4(1), &m_transform);
	}

	m_model->Draw(m_transform, scene->getCameraMatrix(), m_shader);
	
}
