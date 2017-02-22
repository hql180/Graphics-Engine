#include "FBXModel.h"
#include "gl_core_4_4.h"


FBXModel::FBXModel()
{
}


FBXModel::~FBXModel()
{
}

void FBXModel::createOpenGLBuffers(FBXFile * fbx)
{
	
}

void FBXModel::cleanupOpenGLBuffers(FBXFile * fbx)
{
	
}

bool FBXModel::Load(const char * fileName)
{
	m_fbx = new FBXFile();
	m_fbx->load(fileName);

	return true;
}
