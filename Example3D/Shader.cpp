#include <gl_core_4_4.h>
#include <iostream>
#include <fstream>
#include <string>
#include <intrin.h>
#include "Shader.h"


bool Shader::LoadShaderFromFile(const char* filePath, std::string& code)
{
	code = "";

	// Read the Vertex Shader code from the file
	std::ifstream VertexShaderStream(filePath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string line;
		while (getline(VertexShaderStream, line))
			code += line + "\n";
		VertexShaderStream.close();
		return true;
	}
	else {
		printf("Cannot open shader file %s.\n", filePath);
		return false;
	}
}

Shader::Shader()
{
}

Shader::~Shader()
{
}

GLuint Shader::CompileShaders(const char* vsFile, const char* fsFile)
{
	std::string vsCode, fsCode;
	if (!(LoadShaderFromFile(vsFile, vsCode) && LoadShaderFromFile(fsFile, fsCode)))
		printf("failed to load shaders");
	const char* vsSource = vsCode.c_str();
	const char* fsSource = fsCode.c_str();

	GLuint programID;
	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return programID;
}

void Shader::SetShader(const char * vsFile, const char * fsFile)
{
	m_programID = CompileShaders(vsFile, fsFile);
}

unsigned int Shader::GetID()
{
	return m_programID;
}

void Shader::SetID(unsigned int programID)
{
	m_programID = programID;
}

void Shader::SetUniform(const glm::vec3& uniform, Uniform uniformName)
{
	unsigned int loc;

	switch (uniformName)
	{
	case Uniform::CAMERAPOS:
		loc = glGetUniformLocation(m_programID, "cameraPos");
		glUniform3f(loc, uniform.x, uniform.y, uniform.z);
		break;
	case Uniform::LIGHTDIR:
		loc = glGetUniformLocation(m_programID, "lightDirection");
		glUniform3f(loc, uniform.x, uniform.y, uniform.z);
		break;
	case Uniform::LIGHTCOL:
		loc = glGetUniformLocation(m_programID, "lightColour");
		glUniform3f(loc, uniform.x, uniform.y, uniform.z);
		break;
	default:
		std::cout << "Failed to set vec3 uniform" << std::endl;
	}
}

void Shader::SetUniform(const float& uniform, Uniform uniformName)
{
	unsigned int loc;

	switch (uniformName)
	{
	case Uniform::SPECPOWER:
		loc = glGetUniformLocation(m_programID, "specPow");
		glUniform1f(loc, uniform);
		break;
	case Uniform::LIGHTINTENSITY:
		loc = glGetUniformLocation(m_programID, "lightIntensity");
		glUniform1f(loc, uniform);
		break;
	default:
		std::cout << "Failed to set float uniform" << std::endl;
	}
}

void Shader::SetUniform(const glm::mat4& uniform, Uniform uniformName)
{
	unsigned int loc;

	switch (uniformName)
	{
	case MVP:
		loc = glGetUniformLocation(m_programID, "MVP");
		glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&uniform);
		break;
	case MODELTRANSFORM:
		loc = glGetUniformLocation(m_programID, "m");
		glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&uniform);
		break;
	default:
		std::cout << "Failed to set matrix4 uniform" << std::endl;
		break;
	}
}
