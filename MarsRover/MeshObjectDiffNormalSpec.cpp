#include "MeshObjectDiffNormalSpec.h"
#include <cassert>
#include "Matrix4.h"

MeshObjectDiffNormalSpec::MeshObjectDiffNormalSpec(const char * modelpath, MaterialObject& material_object) :
	MeshObject(modelpath),
	TObjectDiff(material_object.TObjectDiffuse0),
	TObjectNorm(material_object.TObjectNormal0),
	TObjectSpec(material_object.TObjectSpecular0)
{
	ImportModelAndSetVBOIBO(modelpath);
}

MeshObjectDiffNormalSpec::~MeshObjectDiffNormalSpec()
= default;

void MeshObjectDiffNormalSpec::DrawObject(Matrix4f &transf, GLuint & shader_program) const
{
	// Set transformations
	const GLint trULocation = glGetUniformLocation(shader_program, "transformation");
	assert(trULocation != -1);
	glUniformMatrix4fv(trULocation, 1, false, transf.get());

	Matrix4f normalMatrix = transf.getInverse().getTransposed();
	const GLint nmaULocation = glGetUniformLocation(shader_program, "normal_matrix");
	glUniformMatrix4fv(nmaULocation, 1, false, normalMatrix.get()); // <-- this normalization bool caused a lot of headache!!!
																	// "true" made the lightsource rotate with the model!

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// set the active texture units

	// Bind the diffuse texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TObjectDiff);

	// Bind the normal texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TObjectNorm);

	//Bind the specular texture in Texture Unit 2
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