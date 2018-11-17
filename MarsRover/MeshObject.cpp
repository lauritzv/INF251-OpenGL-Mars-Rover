#include "MeshObject.h"
#include <iostream>

MeshObject::MeshObject(const char * &modelpath)
{
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
