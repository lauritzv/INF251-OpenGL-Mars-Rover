#pragma once
#include "MeshObject.h"
#include "Matrix4.h"

class MeshObjectDiffNormalSpec : public MeshObject
{
public:
	MeshObjectDiffNormalSpec(const char* modelpath, MaterialObject& material_object);
	~MeshObjectDiffNormalSpec();
	void DrawObject(Matrix4f& transf, GLuint& shader_program) const override;
	//void DrawObject() const override ;

protected:
	GLuint &TObjectDiff;
	GLuint &TObjectNorm;
	GLuint &TObjectSpec;
};
