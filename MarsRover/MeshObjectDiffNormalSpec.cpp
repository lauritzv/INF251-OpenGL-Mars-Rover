#include "MeshObjectDiffNormalSpec.h"
#include <cassert>
#include "Matrix4.h"
#include "RenderProperties.h"

MeshObjectDiffNormalSpec::MeshObjectDiffNormalSpec(const char * modelpath, MaterialObject& material_object, GLuint &sp) :
	MeshObject(modelpath, sp),
	TObjectDiff(material_object.TObjectDiffuse0),
	TObjectNorm(material_object.TObjectNormal0),
	TObjectSpec(material_object.TObjectSpecular0),
	specColor(material_object.specular),
	tint(material_object.tint)
{
	ImportModelAndSetVBOIBO(modelpath);
}

MeshObjectDiffNormalSpec::~MeshObjectDiffNormalSpec()
= default;

void MeshObjectDiffNormalSpec::DrawObject(Matrix4f &transf, RenderProperties& rp) const
{
	// change program and reset if needed, bind buffers and set transform and projection
	DrawObjectCommonPre(transf, rp);

	// set shader-specific uniforms:

	Matrix4f normalMatrix = transf.getInverse().getTransposed();
	const GLint nmaULocation = glGetUniformLocation(shader_program, "normal_matrix");
	glUniformMatrix4fv(nmaULocation, 1, false, normalMatrix.get()); // <-- this normalization bool caused a lot of headache!!!
																	// "true" made the lightsource rotate with the model!
	// Set lightPositionMatrix
	//const GLint lpULocation = glGetUniformLocation(shader_program, "lightPositionMat");
	//glUniformMatrix4fv(lpULocation, 1, false, Matrix4f().get()); //identity matrix

	const GLint vmULocation = glGetUniformLocation(shader_program, "viewMode");
	//assert(vmULocation != -1);
	glUniform1i(vmULocation, rp.viewMode);

	// tell the shader which T.U. to use
	GLint const diffSamplerULocation = glGetUniformLocation(shader_program, "diffSampler");
	glUniform1i(diffSamplerULocation, 0);
	GLint const normSamplerULocation = glGetUniformLocation(shader_program, "normSampler");
	glUniform1i(normSamplerULocation, 1);
	GLint const specSamplerULocation = glGetUniformLocation(shader_program, "specSampler");
	glUniform1i(specSamplerULocation, 2);



	// set the active uniforms and texture units

	const GLint hasDiffULocation = glGetUniformLocation(shader_program, "hasDiffuseMap");
	//assert(hasDiffULocation != -1);
	if (TObjectDiff != -1)
	{
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, TObjectDiff);
		glUniform1i(hasDiffULocation, 1);
	}
	else glUniform1i(hasDiffULocation, 0);

	const GLint hasNormalULocation = glGetUniformLocation(shader_program, "hasNormalMap");
	//assert(hasNormalULocation != -1);
	if (TObjectNorm != -1)
	{
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, TObjectNorm);
		glUniform1i(hasNormalULocation, 1);
	}
	else glUniform1i(hasNormalULocation, 0);

	const GLint hasSpecularULocation = glGetUniformLocation(shader_program, "hasSpecularMap");
	//assert(hasSpecularULocation != -1);
	if (TObjectSpec != -1)
	{
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, TObjectSpec);
		glUniform1i(hasSpecularULocation, 1);
	}
	else glUniform1i(hasSpecularULocation, 0);

	const GLint specColorULocation = glGetUniformLocation(shader_program, "specularColor");
	//assert(specColorULocation != -1);
	glUniform3fv(specColorULocation, 1, specColor.get());

	const GLint tintULocation = glGetUniformLocation(shader_program, "diffuseTint");
	//assert(tintULocation != -1);
	glUniform3fv(tintULocation, 1, tint.get());

	const GLint roverHeadLightPosULocation = glGetUniformLocation(shader_program, "headlightPos");
	//assert(roverHeadLightPosULocation != -1);
	glUniform3fv(roverHeadLightPosULocation, 1, rp.headlightPos.get());

	const GLint roverHeadLightTargULocation = glGetUniformLocation(shader_program, "headlightTarg");
	//assert(roverHeadLightTargULocation != -1);
	glUniform3fv(roverHeadLightTargULocation, 1, rp.headlightTarg.get());


	// set the active texture units

	 //Bind the diffuse texture in Texture Unit 0
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