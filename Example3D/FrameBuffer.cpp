#include "FrameBuffer.h"
#include "gl_core_4_4.h"


FrameBuffer::FrameBuffer()
{
}


FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::SetUp()
{
	// generate a new framebuffer and bind it as current frame buffer

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// generate a new texture and bind it as current

	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	// allocate memory for desired screen output with 8 bytes each on GPU

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, (GLsizei)width, (GLsizei)height);

	// set up filtering parameters for texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attaching texture to framebuffer

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

	// create new depth buffer and set as current

	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);

	// allocate memory for depth buffer

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (GLsizei)width, (GLsizei)height);

	// attach depth buffer to current frame buffer

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);

	// checks to test if everything's fine and dandy

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");

	// detach frame buffer from current so 

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// render buffers?
}

void FrameBuffer::RecreateBuffer(float w, float h)
{
	width = w;
	height = h;

	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_fboTexture);
	glDeleteRenderbuffers(1, &m_fboDepth);		

	SetUp();
}

void FrameBuffer::SetQuad(Model * model)
{
	m_model = model;
}

void FrameBuffer::RenderScene(Scene & scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glClearColor(0.75f, 0.75f, 0.75f, 1); // could pass this in as argument, or ignore altogether

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.draw();

	// restore normal frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLsizei)scene.m_screenWidth, (GLsizei)scene.m_screenHeight);	
	glClearColor(0.0f, 0.0f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::Draw(Shader * shader)
{
	unsigned int programID = shader->GetID();

	glUseProgram(programID);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	int loc = glGetUniformLocation(programID, "target");
	glUniform1i(loc, 0);

	glBindVertexArray(m_model->m_glInfo[0].m_VAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
