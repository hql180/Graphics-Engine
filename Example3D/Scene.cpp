#include "Scene.h"
#include "gl_core_4_4.h"
#include "MyApplication.h"

Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::draw()
{
	for (auto& instance : m_instances)
	{
		instance->draw(this);
	}
}

void Scene::useShader(unsigned int shader)
{
	unsigned int loc;

	glUseProgram(shader);

	loc = glGetUniformLocation(shader, "cameraPos");

	glUniform3f(loc, m_camera.position.x, m_camera.position.y, m_camera.position.z);

	loc = glGetUniformLocation(shader, "lightDirection");
	
	glUniform3f(loc, m_lightDir.x, m_lightDir.y, m_lightDir.z);
}

glm::mat4 Scene::getCameraMatrix()
{
	return m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix(app->getWindowWidth(), app->getWindowHeight);
}
