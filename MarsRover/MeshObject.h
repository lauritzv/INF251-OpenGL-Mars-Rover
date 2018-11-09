#pragma once
#include <GL/glew.h>
#include "model_obj.h"

class MeshObject
{
public:
	bool successfullyImported = true;
	MeshObject(const char* modelpath, GLuint& tobjectdiff, GLuint& tobjectnorm, GLuint& tobjectspec);
	void DrawObject() const;
private:
	GLuint &TObjectDiff;
	GLuint &TObjectNorm;
	GLuint &TObjectSpec;
	ModelOBJ Model;		///< A 3D model
	GLuint VBO = 0;		///< A vertex buffer object
	GLuint IBO = 0;		///< An index buffer object
	GLuint TObjectDiffuse = -1, TObjectNormal = -1, TObjectSpecular = -1;
};
