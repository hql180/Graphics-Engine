#include "Model.h"
#include "Texture.h"
#include "gl_core_4_4.h"

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
		m_fbx->load(filename, FBXFile::UNITS_METER, false, false); 
		createOpenGLBuffers(m_fbx);
		isFBX = true;
		return true;
	}

	return false;
}

bool Model::Load(const char * modelFile, const char * textureFile)
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

void Model::Draw(glm::mat4 transform, glm::mat4 cameraMatrix, unsigned int programID)
{
	mat4 mvp = cameraMatrix * transform;

	unsigned int projectionViewUniform = glGetUniformLocation(programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, (float*)&mvp);

	unsigned int modelUniform = glGetUniformLocation(programID, "m");
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (float*)&transform);

	if (m_texture != NULL)
	{	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		unsigned int textureUniform = glGetUniformLocation(programID, "tex");
		glUniform1i(textureUniform, 0);
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
		glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO);

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
		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO);
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

		glBindVertexArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
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