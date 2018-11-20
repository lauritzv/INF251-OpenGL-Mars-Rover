#pragma once
#include "MeshObject.h"
class MeshObjectUnlitDiffuse :
	public MeshObject
{
public:
	MeshObjectUnlitDiffuse(const char* modelpath, MaterialObject& material_object);
	~MeshObjectUnlitDiffuse();
	void DrawObject(Matrix4f &transf, GLuint &shader_program) const override;

protected:
	GLuint &TObjectDiff;
};
