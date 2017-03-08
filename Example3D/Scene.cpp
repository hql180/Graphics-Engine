#include "Scene.h"
#include "gl_core_4_4.h"
#include "MyApplication.h"
#include "glfw\include\GLFW\glfw3.h"
#include "Application.h"
#include "Gizmos.h"

Scene::Scene()
{
	m_specPow = 20;
	m_lightIntensity = 1.0f;
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

	aie::Gizmos::draw(getCameraMatrix());
}

void Scene::useShader(Shader* shader)
{
	unsigned int ID = shader->GetID();

	glUseProgram(ID);

	shader->SetUniform(m_camera.position, Uniform::CAMERAPOS);

	shader->SetUniform(m_lightDir, Uniform::LIGHTDIR);
	
	shader->SetUniform(m_lightColour, Uniform::LIGHTCOL);

	shader->SetUniform(m_specPow, Uniform::SPECPOWER);

	shader->SetUniform(m_lightIntensity, Uniform::LIGHTINTENSITY);
}

void Scene::update(float dt, float screenHeight, float screenWidth, Camera& camera, bool orbitingLight)
{
	for (auto& instance : m_instances)
	{
		instance->updateTransform();
		instance->m_model->Update(dt);		
	}
	
	if (orbitingLight)
		m_lightDir = glm::vec3(sin(glfwGetTime()), m_lightDir.y, cos(glfwGetTime()));

	m_lightColour = glm::vec3(1, 1, 1);

	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;

	m_camera = camera;
}

glm::mat4 Scene::getCameraMatrix()
{
	glm::mat4 PM = m_camera.GetProjectionMatrix(m_screenWidth, m_screenHeight);
	glm::mat4 cameraMat = (PM * m_camera.GetViewMatrix());
	return cameraMat;
}
