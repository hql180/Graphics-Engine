#include "MyApplication.h"
#include "Gizmos.h"
#include "glm\glm\ext.hpp"
#include "glm\glm\matrix.hpp"
#include "Input.h"
#include "gl_core_4_4.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "FBXLoader\FBXFile.h"
#include <iostream>
#include "glfw\include\GLFW\glfw3.h"
#include "Instance.h"
#include "imgui\imgui.h"

#define STB_IMAGE_IMPLEMENTATION 

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

MyApplication::MyApplication()
{
}

MyApplication::~MyApplication()
{
}

bool MyApplication::startup()
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);



	//// shader

	//int success = GL_FALSE;

	////create a new vertex and fragment shader, and get tickets for them
	//unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//// set the source for the vertext shader to our code strng above, and compile
	//glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	//glCompileShader(vertexShader);
	//// and the same for the fragement shader
	//glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	//glCompileShader(fragmentShader);

	//// create a new shader pipeline and store the ticket for it
	//m_programID = glCreateProgram();

	//// attach vertex and fragment shaders to shader pipeline these are fitted according to flags made at their creation
	//glAttachShader(m_programID, vertexShader);
	//glAttachShader(m_programID, fragmentShader);
	//// link the whole pipeline
	//glLinkProgram(m_programID);
	//// checks for error and return message if one was found
	//glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	//if (success == GL_FALSE)
	//{
	//	int infoLogLength = 0; 
	//	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength); 
	//	char* infoLog = new char[infoLogLength]; 
	//	glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog); 
	//	printf("Error: Failed to link shader program!\n"); 
	//	printf("%s\n", infoLog); delete[] infoLog;
	//}

	//// free memory shaders only need linked pipeline
	//glDeleteShader(fragmentShader);
	//glDeleteShader(vertexShader);

	
	// testModel.Load("../Example3D/Models/Iron_Man.obj", "../Example3D/Models/Iron_Man.tga");

	Shader temp1;
	temp1.SetShader(vsSource, fsSource);
	m_shaders.push_back(temp1);

	Shader temp2;
	temp2.SetShader("../Example3D/Shaders/animatedVertexShader.vert", fsSource);
	m_shaders.push_back(temp2);

	Shader frameBuffShader;
	frameBuffShader.SetShader("../Example3D/Shaders/frameBuffer.vert", "../Example3D/Shaders/frameBuffer.frag");
	m_shaders.push_back(frameBuffShader);


	if (!pyro.Load("../Example3D/Models/Pyro/pyro.fbx",
		"../Example3D/Models/Pyro/Pyro_D.tga",
		"../Example3D/Models/Pyro/Pyro_N.tga",
		"../Example3D/Models/Pyro/Pyro_S.tga"))
		return false;

	if (!buns.Load("../Example3D/Models/Bunny.obj"))
		return false;

	
	
	for (int i = 0; i < 500; ++i)
	{
		m_scene.m_instances.push_back(new Instance(&pyro, &m_shaders[1], vec3((float)glm::cos(i)*2.0f, (float)glm::sin(i)*2.0f, (float)glm::cos(i)*(float)glm::sin(i)*2.0f), vec3((float)glm::tan(i)*19.0f), vec3(0.001f)));
	}	
	
	

	m_orbitOn = true;

	m_distort = false;

	m_sharpen = false;

	m_sobel = false;

	m_blur = false;

	m_radialBlur = false;

	//m_scene.m_instances.push_back(new Instance(&screenQuad, &m_shaders[2]));

	//pFrameBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
	//pFrameBuffer->SetUp();

	float width = (float)getWindowWidth();
	float height = (float)getWindowHeight();

	pFrameBuffer = new FrameBuffer(width, height);
	pFrameBuffer->SetUp();

	/*Model* quadModel = new Model();
	quadModel->makePostProcessQuad(getWindowWidth(), getWindowHeight());
	pFrameBuffer->SetQuad(quadModel);*/

	Model* quadModel = new Model();
	quadModel->makePostProcessQuad(width, height);
	pFrameBuffer->SetQuad(quadModel);

	m_prevWidth = width;
	m_prevHeight = height;

	//for (int i = 0; i < 500; ++i)
	//{
	//	m_scene.m_instances.push_back(new Instance(&buns, &m_shaders[0], vec3(0.1f*i, 0, 0.1f*i), vec3(0.1f*i), vec3(0.5f)));

	//}

	return true;
}

void MyApplication::shutdown()
{
	Gizmos::destroy();
}

void MyApplication::update(float dt)
{
	m_time = getTime();

	camera.Update();

	float width = (float)getWindowWidth();

	float height = (float)getWindowHeight();

	m_scene.update(m_time, height, width, camera, m_orbitOn);

	if (m_prevWidth != width || m_prevHeight != height)
	{
		pFrameBuffer->RecreateBuffer(width, height);
		//delete pFrameBuffer->m_model;
		//delete pFrameBuffer;
		//pFrameBuffer = new FrameBuffer(getWindowWidth(), getWindowHeight());
		//pFrameBuffer->SetUp();
		//Model* quadModel = new Model();
		//quadModel->makePostProcessQuad(getWindowWidth(), getWindowHeight());
		//pFrameBuffer->SetQuad(quadModel);

		m_prevWidth = width;
		m_prevHeight = height;
	}

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}


	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	//// demonstrate a few shapes
	//Gizmos::addAABBFilled(vec3(0), vec3(1), vec4(0, 0.5f, 1, 0.25f));
	//Gizmos::addSphere(vec3(5, 0, 5), 1, 8, 8, vec4(1, 0, 0, 0.5f));
	//Gizmos::addRing(vec3(5, 0, -5), 1, 1.5f, 8, vec4(0, 1, 0, 1));
	//Gizmos::addDisk(vec3(-5, 0, 5), 1, 16, vec4(1, 1, 0, 1));
	//Gizmos::addArc(vec3(-5, 0, -5), 0, 2, 1, 8, vec4(1, 0, 1, 1));

	//mat4 t = glm::rotate(m_time, glm::normalize(vec3(1, 1, 1)));
	//t[3] = vec4(-2, 0, 0, 1);
	//Gizmos::addCylinderFilled(vec3(0), 0.5f, 1, 5, vec4(0, 1, 1, 1), &t);

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void MyApplication::draw()
{
	clearScreen();	

	float width = (float)getWindowWidth();
	float height = (float)getWindowHeight();

	m_projectionMatrix = camera.GetProjectionMatrix(width, height);

	m_viewMatrix = camera.GetViewMatrix();

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);

	//m_scene.draw();

	pFrameBuffer->RenderScene(m_scene);
	pFrameBuffer->Draw(&m_shaders[2]);

	// frameBuffer Uniforms
	m_shaders[2].SetUniform(m_distort, Uniform::DISTORT);
	m_shaders[2].SetUniform(m_sharpen, Uniform::SHARPEN);
	m_shaders[2].SetUniform(m_blur, Uniform::BLUR);
	m_shaders[2].SetUniform(m_sobel, Uniform::SOBEL);
	m_shaders[2].SetUniform(m_radialBlur, Uniform::RADIALBLUR);
	m_shaders[2].SetUniform(m_scene.m_lightDir, Uniform::LIGHTDIR);
	m_shaders[2].SetUniform(m_projectionMatrix * m_viewMatrix, MVP);

	m_shaders[2].SetUniform(m_time, "iGlobalTime");

	ImGui::Begin("Lights");
	ImGui::Checkbox("Auto Orbit Light", &m_orbitOn);
	ImGui::SliderFloat3("Light Pos", (float*)&m_scene.m_lightDir, -10, 10);
	ImGui::SliderFloat3("Point Light Pos", &m_scene.m_pointLights[0].x, -20, 20);
	ImGui::SliderFloat3("Point Light Color", &m_scene.m_pointLightColours[0].x, 0, 1);
	ImGui::SliderFloat("Point Light Power", &m_scene.m_pointLightPowers[0], 0, 100);
	ImGui::End();

	ImGui::Begin("Post-Processing");
	ImGui::Checkbox("Distort", &m_distort);
	ImGui::Checkbox("Blur", &m_blur);
	ImGui::Checkbox("Sharpen", &m_sharpen);
	ImGui::Checkbox("Sobel Operator", &m_sobel);
	ImGui::Checkbox("Radial Blur", &m_radialBlur);
	
	ImGui::End();

	ImGui::Begin("Fustum Culling");
	ImGui::Checkbox("Show Bounding Boxes", &m_scene.showBounds);
	ImGui::DragInt("Culled", &m_scene.culled);
	ImGui::DragInt("FPS", (int*)&m_fps);
	ImGui::End();
}

//void MyApplication::generateGrid(unsigned int rows, unsigned int cols)
//{
//	Vertex* aoVertices = new Vertex[rows * cols];
//
//	for (unsigned int r = 0; r < rows; ++r)
//	{
//		for (unsigned int c = 0; c < cols; c++)
//		{
//			aoVertices[r*cols + c].position = vec4((float)c, 0, (float)r, 1);
//
//			// create some arbitrary colour based off something
//			vec3 colour = vec3(sinf(c / (float(cols - 1))*
//				(r / (float)(rows - 1))));
//
//			aoVertices[r*cols + c].colour = vec4(colour, 1);
//		}
//	}
//
//	// defining index count based off quad count(2 triangles per quad)
//	unsigned int* auiIndices = new unsigned int[(rows - 1)*(cols - 1) * 6];
//
//	unsigned int index = 0;
//
//	for (unsigned int r = 0; r < (rows - 1); ++r)
//	{
//		for (unsigned int c = 0; c < (cols - 1); c++)
//		{
//			// triangle 1
//			auiIndices[index++] = r*cols + c;
//			auiIndices[index++] = (r + 1)*cols + c;
//			auiIndices[index++] = (r + 1)*cols + (c + 1);
//
//			// triangle 2
//			auiIndices[index++] = r * cols + c; 
//			auiIndices[index++] = (r + 1) * cols + (c + 1); 
//			auiIndices[index++] = r * cols + (c + 1);
//		}
//	}
//
//	// create vertex and index buffers and store
//	glGenBuffers(1, &m_VBO);
//	glGenBuffers(1, &m_IBO);
//
//	//create and bind buffers to a vertex array object and store a ticket for it
//	glGenVertexArrays(1, &m_VAO); // may need to flip this
//
//	// set the current vertex array object to the one we created
//	glBindVertexArray(m_VAO);
//
//	// set current vertext buffer object to the one we created
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//
//	// feed columns array to it
//	glBufferData(GL_ARRAY_BUFFER, (rows*cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
//
//	// turn on slots 0 and 1 in this vertex buffer object
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//
//	// select buffer slot 0 and make it start a 0 (start) with each vertex seperated in memory by the size of vertex
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	// select buffer slot 1 and start one vec4 into the data with each vertext seperated in memory separated by size of vertex
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
//	// this results in a striped data block which goes vertex, colour, vertex, etc
//
//	// set current index buffer to the one created previously and associates it with current VAO
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
//	// feed in array indices
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
//	
//	// set the current VOA to zero now that data setup is completed
//	glBindVertexArray(0);
//
//	// remember to unbind theses after unbinding current VAO
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	glDeleteBuffers(1, &m_IBO);
//	glDeleteBuffers(1, &m_VBO);
//
//	// vertex data is now copied into graphics card memory and have a ticket for it
//	// can now delete memory on the CPU side now that its no longer needed
//	delete[] aoVertices;
//}



