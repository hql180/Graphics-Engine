#pragma once
#include "FBXLoader\FBXFile.h"
#include "Model.h"

class FBXModel :
	public Model
{
public:
	FBXModel();
	~FBXModel();



	unsigned int m_program;
};

