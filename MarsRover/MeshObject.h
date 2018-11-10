#pragma once
#include <GL/glew.h>
#include "model_obj.h"
#include "MaterialObject.h"

class MeshObject
{
public:
	bool successfullyImported = true;
	MeshObject(const char* modelpath, MaterialObject& material_object);
	void DrawObject() const;
private:
	void ImportModelAndSetVBOIBO(const char * &modelpath);
	GLuint &TObjectDiff;
	GLuint &TObjectNorm;
	GLuint &TObjectSpec;
	ModelOBJ Model;		///< A 3D model
	GLuint VBO = 0;		///< A vertex buffer object
	GLuint IBO = 0;		///< An index buffer object
};
