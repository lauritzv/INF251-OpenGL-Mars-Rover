#pragma once
#include "MeshObject.h"
class MeshObjectUnlitDiffuse :
	public MeshObject
{
public:
	MeshObjectUnlitDiffuse(const char* modelpath, MaterialObject& material_object);
	~MeshObjectUnlitDiffuse();
	void DrawObject() const override;

protected:
	GLuint &TObjectDiff;
};
