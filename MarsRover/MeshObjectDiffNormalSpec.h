#pragma once
#include "MeshObject.h"
#include "Matrix4.h"

class MeshObjectDiffNormalSpec : public MeshObject
{
public:
	MeshObjectDiffNormalSpec(const char* modelpath, MaterialObject& material_object, GLuint& sp);
	~MeshObjectDiffNormalSpec();
	void DrawObject(Matrix4f& transf, RenderProperties& rp) const override;

protected:
	GLuint &TObjectDiff;
	GLuint &TObjectNorm;
	GLuint &TObjectSpec;

	Vector3f& specColor;
	Vector3f& tint;
};
