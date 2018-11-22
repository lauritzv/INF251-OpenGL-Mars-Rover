#include "MeshObject.h"
#include <iostream>

/// change program and reset if needed, bind buffers and set transform and projection
void MeshObject::DrawObjectCommonPre(Matrix4f & transf, Matrix4f & projection) const
{
	GLint current_program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

	if (shader_program != current_program)
	{
		glUseProgram(0);
		assert(shader_program != 0);
		glUseProgram(shader_program);
	}
	// Set projection
	const GLint prULocation = glGetUniformLocation(shader_program, "projection");
	assert(prULocation != -1);
	glUniformMatrix4fv(prULocation, 1, false, projection.get());

	// Set transformations
	const GLint trULocation = glGetUniformLocation(shader_program, "transformation");
	assert(trULocation != -1);
	glUniformMatrix4fv(trULocation, 1, false, transf.get());

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
}

/// Bind vert attribs and draw
void MeshObject::DrawObjectCommonPost() const
{
	//position
	glVertexAttribPointer(
		0,											//location
		3,											//datacount per
		GL_FLOAT,									//datatype
		GL_FALSE,									//normalize?
		sizeof(ModelOBJ::Vertex),					//stride (vertsize)
		reinterpret_cast<const GLvoid*>(0));		//attrib offset

	//texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(3 * sizeof(float))); //offset by pos(3)

	//normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(5 * sizeof(float)) //offset by pos(3)+texcoord(2)
	);

	// Draw the elements on the GPU
	glDrawElements(
		GL_TRIANGLES,
		Model.getNumberOfIndices(),
		GL_UNSIGNED_INT,
		0);
}

MeshObject::MeshObject(const char * &modelpath, GLuint& sp)
{
	shader_program = sp;
	ImportModelAndSetVBOIBO(modelpath);
}

void MeshObject::ImportModelAndSetVBOIBO(const char * &modelpath)
{
	successfullyImported = Model.import(modelpath);
	if (!successfullyImported)
	{
		std::cerr << "Error: cannot load model: " << modelpath << "." << std::endl;
	}
	else
	{
		// VBO
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER,
			Model.getNumberOfVertices() * sizeof(ModelOBJ::Vertex),
			Model.getVertexBuffer(),
			GL_STATIC_DRAW);

		// IBO
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			Model.getNumberOfTriangles() * 3 * sizeof(unsigned int),
			Model.getIndexBuffer(),
			GL_STATIC_DRAW);
	}
}
