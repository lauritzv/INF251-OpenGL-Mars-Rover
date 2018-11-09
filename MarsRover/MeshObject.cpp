#include "MeshObject.h"
#include <iostream>

MeshObject::MeshObject(const char * modelpath, GLuint &tobjectdiff, GLuint &tobjectnorm, GLuint &tobjectspec) :
	TObjectDiff(tobjectdiff), TObjectNorm(tobjectnorm), TObjectSpec(tobjectspec)
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
		             3 * Model.getNumberOfTriangles() * sizeof(unsigned int),
		             Model.getIndexBuffer(),
		             GL_STATIC_DRAW);
	}
}

void MeshObject::DrawObject() const
{
	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// set the active texture units

	// Bind our diffuse texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TObjectDiff);

	// Bind our normal texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TObjectNorm);

	//Bind our specular texture in Texture Unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TObjectSpec);

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
