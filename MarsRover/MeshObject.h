#pragma once
#include <GL/glew.h>
#include "model_obj.h"
#include "MaterialObject.h"
#include "Matrix4.h"

class RenderProperties;

class MeshObject
{
public:
	virtual ~MeshObject() = default;
	bool successfullyImported = true;
	virtual void DrawObject(Matrix4f &transf, RenderProperties &rp) const = 0;
	void SetShaderProgram(GLuint &sp) { shader_program = sp; }

protected:
	void DrawObjectCommonPre(Matrix4f &transf, RenderProperties& rp) const;
	void DrawObjectCommonPost() const;
	GLuint shader_program;
	MeshObject(const char*& modelpath, GLuint& sp);
	void ImportModelAndSetVBOIBO(const char * &modelpath);
	ModelOBJ Model;		///< A 3D model
	GLuint VBO = 0;		///< A vertex buffer object
	GLuint IBO = 0;		///< An index buffer object
};
