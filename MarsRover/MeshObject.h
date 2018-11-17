#pragma once
#include <GL/glew.h>
#include "model_obj.h"
#include "MaterialObject.h"

class MeshObject
{
public:
	//virtual ~MeshObject() = default;
	//MeshObject();
	//virtual ~MeshObject() = 0;
	bool successfullyImported = true;
	virtual void DrawObject() const = 0;

private:

protected:
	MeshObject(const char*& modelpath);
	void ImportModelAndSetVBOIBO(const char * &modelpath);
	ModelOBJ Model;		///< A 3D model
	GLuint VBO = 0;		///< A vertex buffer object
	GLuint IBO = 0;		///< An index buffer object
};
