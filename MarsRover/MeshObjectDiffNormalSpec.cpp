#include "MeshObjectDiffNormalSpec.h"
#include <cassert>
#include "Matrix4.h"

MeshObjectDiffNormalSpec::MeshObjectDiffNormalSpec(const char * modelpath, MaterialObject& material_object, GLuint &sp) :
	MeshObject(modelpath, sp),
	TObjectDiff(material_object.TObjectDiffuse0),
	TObjectNorm(material_object.TObjectNormal0),
	TObjectSpec(material_object.TObjectSpecular0)
{
	ImportModelAndSetVBOIBO(modelpath);
}

MeshObjectDiffNormalSpec::~MeshObjectDiffNormalSpec()
= default;

void MeshObjectDiffNormalSpec::DrawObject(Matrix4f &transf, Matrix4f &projection) const
{
	// change program and reset if needed, bind buffers and set transform and projection
	DrawObjectCommonPre(transf, projection);

	// set shader-specific uniforms:

	Matrix4f normalMatrix = transf.getInverse().getTransposed();
	const GLint nmaULocation = glGetUniformLocation(shader_program, "normal_matrix");
	glUniformMatrix4fv(nmaULocation, 1, false, normalMatrix.get()); // <-- this normalization bool caused a lot of headache!!!
																	// "true" made the lightsource rotate with the model!
	// Set lightPositionMatrix
	const GLint lpULocation = glGetUniformLocation(shader_program, "lightPositionMat");
	glUniformMatrix4fv(lpULocation, 1, false, Matrix4f().get()); //identity matrix

	const GLint vmULocation = glGetUniformLocation(shader_program, "viewMode");
	assert(vmULocation != -1);
	glUniform1i(vmULocation, 0);

	// tell the shader which T.U. to use
	GLint const normSamplerULocation = glGetUniformLocation(shader_program, "normSampler");
	glUniform1i(normSamplerULocation, 1);
	GLint const specSamplerULocation = glGetUniformLocation(shader_program, "specSampler");
	glUniform1i(specSamplerULocation, 2);
	GLint const diffSamplerULocation = glGetUniformLocation(shader_program, "diffSampler");
	glUniform1i(diffSamplerULocation, 0);

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

	//Bind vert attribs and draw
	DrawObjectCommonPost();
}