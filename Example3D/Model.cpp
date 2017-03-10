#include "Model.h"
#include "Texture.h"
#include "gl_core_4_4.h"
#include <algorithm>

using glm::mat4;

Model::Model()
{
}


Model::~Model()
{
}

bool Model::Load(const char * filename)
{
	if (strstr(filename, ".obj") != NULL)
	{
		tinyobj::attrib_t attribs;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, filename);
		createOpenGLBuffers(attribs, shapes);
		isFBX = false;
		return success;
	}

	if (strstr(filename, ".fbx") != NULL)
	{
		m_fbx = new FBXFile(); 
		m_fbx->load(filename, FBXFile::UNIT_SCALE::UNITS_METER, true, true, true); 
		createOpenGLBuffers(m_fbx);
		isFBX = true;
		return true;
	}

	return false;
}

bool Model::Load(const char * modelFile, const char * textureFile)
{
	if (strstr(modelFile, ".obj") != NULL)
	{
		tinyobj::attrib_t attribs;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, modelFile);

		m_texture = Texture::LoadTexture(textureFile);

		createOpenGLBuffers(attribs, shapes);

		return success;
	}

	if (strstr(modelFile, ".fbx") != NULL)
	{
		m_fbx = new FBXFile();
		m_fbx->load(modelFile, FBXFile::UNIT_SCALE::UNITS_METER, true, true, true);
		m_texture = Texture::LoadTexture(textureFile);
		createOpenGLBuffers(m_fbx);
		isFBX = true;


		return true;
	}

	return false;
}

bool Model::Load(const char * modelFile, const char * textureFile, const char * normalMapFile)
{
	if (strstr(modelFile, ".obj") != NULL)
	{
		tinyobj::attrib_t attribs;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, modelFile);

		m_texture = Texture::LoadTexture(textureFile);

		createOpenGLBuffers(attribs, shapes);

		return success;
	}

	if (strstr(modelFile, ".fbx") != NULL)
	{
		m_fbx = new FBXFile();
		m_fbx->load(modelFile, FBXFile::UNIT_SCALE::UNITS_METER, true, true, true);
		m_texture = Texture::LoadTexture(textureFile);
		m_normalmap = Texture::LoadTexture(normalMapFile);
		createOpenGLBuffers(m_fbx);
		isFBX = true;


		return true;
	}

	return false;
}

bool Model::Load(const char * modelFile, const char * textureFile, const char * normalMapFile, const char * specularMapFile)
{
	if (strstr(modelFile, ".obj") != NULL)
	{
		tinyobj::attrib_t attribs;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, modelFile);

		m_texture = Texture::LoadTexture(textureFile);

		createOpenGLBuffers(attribs, shapes);

		return success;
	}

	if (strstr(modelFile, ".fbx") != NULL)
	{
		m_fbx = new FBXFile();
		m_fbx->load(modelFile, FBXFile::UNIT_SCALE::UNITS_METER, true, true, true);
		m_texture = Texture::LoadTexture(textureFile);
		m_normalmap = Texture::LoadTexture(normalMapFile);
		m_specularmap = Texture::LoadTexture(specularMapFile);
		createOpenGLBuffers(m_fbx);
		isFBX = true;


		return true;
	}

	return false;
}

void Model::Draw(glm::mat4 transform, glm::mat4 cameraMatrix, Shader* shader)
{
	mat4 mvp = cameraMatrix * transform;

	shader->SetUniform(mvp, Uniform::MVP);

	shader->SetUniform(transform, Uniform::MODELTRANSFORM);

	unsigned int programID = shader->GetID();

	if (m_texture != NULL)
	{	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		unsigned int textureUniform = glGetUniformLocation(programID, "tex");
		glUniform1i(textureUniform, 0);
	}

	if (m_normalmap != NULL)
	{
		glActiveTexture(GL_TEXTURE1); 
		glBindTexture(GL_TEXTURE_2D, m_normalmap);
		unsigned int normalUniform = glGetUniformLocation(programID, "normal");
		glUniform1i(normalUniform, 1);
	}

	if (m_specularmap != NULL)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_specularmap);
		unsigned int specularUniform = glGetUniformLocation(programID, "specular");
		glUniform1i(specularUniform, 2);
	}

	if (!isFBX)
	{
		for (auto & gl : m_glInfo)
		{
			// bind vao before drawing down below
			glBindVertexArray(gl.m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
		}
	}

	if (isFBX)
	{
		if (isAnimated())
		{
			FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);

			skeleton->updateBones();

			unsigned int bones_location = glGetUniformLocation(programID, "bones");
			glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
		}

		

		//for(auto& gl: m_fbx->)
		for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
			unsigned int* glData = (unsigned int*)mesh->m_userData;
			glBindVertexArray(glData[0]); 
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
}

void Model::createOpenGLBuffers(tinyobj::attrib_t & attribs, std::vector<tinyobj::shape_t>& shapes)
{
	m_glInfo.resize(shapes.size());
	// grab each shape
	// index number for glinfo
	int shapeIndex = 0;
	for (auto& shape : shapes)
	{
		// setup OpenGl data
		// Generates array and buffers @ memory location glinfo[shapeIndex].etc
		glGenVertexArrays(1, &m_glInfo[shapeIndex].m_VAO);
		glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO1);

		// binds current vertex array to voa created above
		glBindVertexArray(m_glInfo[shapeIndex].m_VAO);
		// set face count of current shapeIndex to number of face vertices?????
		m_glInfo[shapeIndex].m_faceCount = shape.mesh.num_face_vertices.size();

		// create tempory storage for objvertices
		std::vector<OBJVertex> vertices;

		// index number for indices in shape mesh
		int index = 0;

		// loops through all face vertices in mesh
		for (auto face : shape.mesh.num_face_vertices)
		{
			// loop once for each tri????
			for (int i = 0; i < 3; ++i)
			{
				// special index struct to help access correct tinyobj::attrib vertices
				tinyobj::index_t idx = shape.mesh.indices[index + i];

				// create an empty OBJVertex set to 0
				OBJVertex v = { 0 };

				// stores respective data from attrib.vertices in the empty objvertex v created above
				// positions 
				v.x = attribs.vertices[3 * idx.vertex_index + 0];
				v.y = attribs.vertices[3 * idx.vertex_index + 1];
				v.z = attribs.vertices[3 * idx.vertex_index + 2];
				// normals 
				if (attribs.normals.size() > 0)
				{
					v.nx = attribs.normals[3 * idx.normal_index + 0];
					v.ny = attribs.normals[3 * idx.normal_index + 1];
					v.nz = attribs.normals[3 * idx.normal_index + 2];
				}
				// texture coordinates 
				if (attribs.texcoords.size() > 0)
				{
					v.u = attribs.texcoords[2 * idx.texcoord_index + 0];
					v.v = attribs.texcoords[2 * idx.texcoord_index + 1];
				}
				// add OBJVertex above to vector list
				vertices.push_back(v);
			}
			// increase index by face (3) because that is the size of each data set??????
			index += face;
		}


		// bind vertex buffer created along with current vao
		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO1);
		// binds all the data stored in vertices vector to be used as the source for GL drawing commands
		// second value being size in bytes of data
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW);

		// enable a gpu slot @#
		glEnableVertexAttribArray(0); //position 
									  // and have it point to data specified by below attribpointer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);

		glEnableVertexAttribArray(1); //normal data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)12);

		glEnableVertexAttribArray(2); //texture data 

		// void* 24 is the number of bytes into data to read ie skipping 2 * 3 sets of floats (4) = 24
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)24);

		// unbinds VAO from current slot as ticket is stored in glinfo[shapeIndex].mVAO
		glBindVertexArray(0);
		// can now safely unbind current buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// shifts index to next shape
		shapeIndex++;
	}

	calculateBounds(attribs);
}

void Model::createOpenGLBuffers(FBXFile * fbx)
{
	// create gl VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		// loading vector of FBXVertex array buffer VBO
		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(),
			GL_STATIC_DRAW);

		// loading vector of unsigned int to index buffer IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(),
			GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal 
									  // FBXvertex::normaloffset defined in fbxfile.h
									  // ((char*)0) tells it to start at 0 and increment 1 byte for each value
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
			
		glEnableVertexAttribArray(2); //texture data 
										// void* 24 is the number of bytes into data to read ie skipping 2 * 3 sets of floats (4) = 24
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		
		if (isAnimated())
		{
			glEnableVertexAttribArray(3); // tangents

			glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);

			glEnableVertexAttribArray(4); // weights
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);

			glEnableVertexAttribArray(5); // indices
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);
		}

		glBindVertexArray(0); 
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				
		mesh->m_userData = glData;
	}

	calculateBounds(fbx);
}

void Model::cleanupOpenGLBuffers(FBXFile * fbx)
{
	// clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void Model::Update(float dt)
{
	if (isAnimated())
	{
		FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
		FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

		skeleton->evaluate(animation, dt);

		for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
		{
			skeleton->m_nodes[bone_index]->updateGlobalTransform();
		}
	}
}

bool Model::isAnimated()
{
	if (m_fbx == nullptr)
		return false;
	return m_fbx && m_fbx->getSkeletonCount() > 0;
}

void Model::makePostProcessQuad(float screenWidth, float screenHeight)
{
	m_glInfo.resize(1);

	// fullscreen quad

	glm::vec2 halfTexel = 1.0f / glm::vec2(screenWidth, screenHeight) * 0.5f;

	float vertexData[] = 
	{ 
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y // may need ,
	};

	glGenVertexArrays(1, &m_glInfo[0].m_VAO);
	glBindVertexArray(m_glInfo[0].m_VAO);
	glGenBuffers(1, &m_glInfo[0].m_VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[0].m_VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6,
		vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1); // UVs
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::calculateBounds(tinyobj::attrib_t & attribs)
{
	float radius = 0;

	for (int k = 0; k < attribs.vertices.size(); k += 3)
	{
		boundsMin.x = std::min(boundsMin.x, attribs.vertices[k]); // x
		boundsMin.y = std::min(boundsMin.y, attribs.vertices[k + 1]); // y
		boundsMin.z = std::min(boundsMin.z, attribs.vertices[k + 2]); // z
		boundsMax.x = std::max(boundsMax.x, attribs.vertices[k]);
		boundsMax.y = std::max(boundsMax.y, attribs.vertices[k + 1]);
		boundsMax.z = std::max(boundsMax.z, attribs.vertices[k + 2]);
		radius = std::max(radius,
			sqrtf(attribs.vertices[k] * attribs.vertices[k] 
				+ attribs.vertices[k + 1] * attribs.vertices[k + 1] 
				+ attribs.vertices[k + 2] * attribs.vertices[k + 2]));
	}
}

void Model::calculateBounds(FBXFile * fbx)
{
	float radius = 0;

	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		
		for (auto& vert : mesh->m_vertices)
		{
			boundsMin.x = std::min(boundsMin.x, vert.position.x); // x
			boundsMin.y = std::min(boundsMin.y, vert.position.y); // y
			boundsMin.z = std::min(boundsMin.z, vert.position.z); // z
			boundsMax.x = std::max(boundsMax.x, vert.position.x);
			boundsMax.y = std::max(boundsMax.y, vert.position.y);
			boundsMax.z = std::max(boundsMax.z, vert.position.z);
			radius = std::max(radius,
				sqrtf(vert.position.x * vert.position.x
					+ vert.position.y * vert.position.y
					+ vert.position.z * vert.position.z));
		}
	}

}

