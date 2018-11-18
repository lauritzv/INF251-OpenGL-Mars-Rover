﻿#pragma once
#include <GL/glew.h>
#include "model_obj.h"
#include "MaterialObject.h"

class MeshObject
{
public:
	virtual ~MeshObject() = default;
	bool successfullyImported = true;
	virtual void DrawObject() const = 0;

protected:
	MeshObject(const char*& modelpath);
	void ImportModelAndSetVBOIBO(const char * &modelpath);
	ModelOBJ Model;		///< A 3D model
	GLuint VBO = 0;		///< A vertex buffer object
	GLuint IBO = 0;		///< An index buffer object
};
