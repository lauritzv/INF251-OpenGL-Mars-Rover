#pragma once
#include "MeshObject.h"
class MeshObjectDiffNormalSpec : public MeshObject
{
public:
	MeshObjectDiffNormalSpec(const char* modelpath, MaterialObject& material_object);
	~MeshObjectDiffNormalSpec();
	void DrawObject() const override ;

protected:
	GLuint &TObjectDiff;
	GLuint &TObjectNorm;
	GLuint &TObjectSpec;
};

