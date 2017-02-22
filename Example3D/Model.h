#pragma once
#include "glm\glm\glm.hpp"
#include "tiny_obj_loader.h"
#include "FBXLoader\FBXFile.h"

#include <vector>

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
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

	// OBJ STUFF
	bool Load(const char * filename);

	bool Load(const char* modelFile, const char* textureFile);

	void Draw(glm::mat4 transform, glm::mat4 cameraMatrix, unsigned int programID);

	void createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>&shapes);

	void createOpenGLBuffers(FBXFile* fbx);

	void cleanupOpenGLBuffers(FBXFile* fbx);

	FBXFile* m_fbx;

	unsigned int m_texture;

	std::vector<OpenGLInfo> m_glInfo;

	bool isFBX;

	// FBX stuff

	//bool Load(const char* a_filename,
	//			FBXFile::UNIT_SCALE a_scale = FBXFile::UNITS_METER,
	//			bool a_loadTextures = true,
	//			bool a_loadAnimations = true,
	//			bool a_flipTextureY = true);

	//unsigned int getMeshCount() const;

	//FBXMeshNode* getMeshbyIndex(unsigned int a_index) const;

};
