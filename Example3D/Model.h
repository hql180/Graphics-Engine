#pragma once
#include "glm\glm\glm.hpp"
#include "tiny_obj_loader.h"
#include "FBXLoader\FBXFile.h"
#include "Shader.h"

#include <vector>

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO1;
	unsigned int m_VBO2;
	unsigned int m_IBO;
	unsigned int m_faceCount;
};

struct OBJVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

class Model
{
public:
	Model();
	~Model();

	bool Load(const char * filename);

	bool Load(const char* modelFile, const char* textureFile);

	bool Load(const char* modelFile, const char* textureFile, const char* normalMapFile);

	bool Load(const char* modelFile, const char* textureFile, const char* normalMapFile, const char* specularMapFile);

	bool Load(unsigned int texture);

	void Draw(glm::mat4 transform, glm::mat4 cameraMatrix, Shader* shader);

	void createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>&shapes);

	void createOpenGLBuffers(FBXFile* fbx);

	void cleanupOpenGLBuffers(FBXFile* fbx);

	void Update(float dt);

	bool isAnimated();

	void makePostProcessQuad(float screenWidth, float screenHeight);

	void calculateBounds(tinyobj::attrib_t & attribs);

	void calculateBounds(FBXFile* fbx);

	

	glm::vec3 boundsMin;
	glm::vec3 boundsMax;

	FBXFile* m_fbx = nullptr;

	std::vector<unsigned int> m_texture;

	unsigned int m_normalmap;

	unsigned int m_specularmap;

	std::vector<OpenGLInfo> m_glInfo;

	bool isFBX;

	bool isEscher;

	float m_timer;

};

