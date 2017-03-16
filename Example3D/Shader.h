#pragma once
#include <string>
#include "glm\glm\glm.hpp"

enum Uniform
{
	CAMERAPOS,
	LIGHTDIR,
	LIGHTCOL,
	SPECPOWER,
	LIGHTINTENSITY,
	MVP,
	MODELTRANSFORM,
	DISTORT,
	SHARPEN,
	SOBEL,
	BLUR,
	RADIALBLUR
};

class Shader
{
public:
	Shader();
	~Shader();

	static unsigned int CompileShaders(const char* vsFile, const char* fsFile);

	void SetShader(const char* vsFile, const char* fsFile);

	unsigned int GetID();

	void SetID(unsigned int programID);

	void SetUniform(const glm::vec3& uniform, Uniform uniformName);

	void SetUniform(const glm::vec4& uniform, const char*uniformName);

	void SetUniform(const float& uniform, Uniform uniformName);

	void SetUniform(const float& uniform, const char* uniformName);

	void SetUniform(const glm::mat4& uniform, Uniform uniformName);

	void SetUniform(const bool& uniform, Uniform uniformName);

private:
	static bool LoadShaderFromFile(const char* filePath, std::string& code);

	unsigned int m_programID;
};