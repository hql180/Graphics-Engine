#pragma once
#include "Scene.h"
#include "Shader.h"
#include "Model.h"

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(int w, int h) : width(w), height(h) { SetUp(); }
	~FrameBuffer();

	void SetUp();

	void SetQuad(Model* model);

	void RenderScene(Scene& scene);

	void Draw(Shader* shader);

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

	int width, height;

	Model* m_model;
};

