#include "MeshObjectUnlitDiffuse.h"

MeshObjectUnlitDiffuse::MeshObjectUnlitDiffuse(const char * modelpath, MaterialObject& material_object, GLuint& sp) :
	MeshObject(modelpath, sp),
	TObjectDiff(material_object.TObjectDiffuse0)
{
	if (material_object.TObjectDiffuse0 == -1)
		tintColor = material_object.tint;
	ImportModelAndSetVBOIBO(modelpath);
}

MeshObjectUnlitDiffuse::~MeshObjectUnlitDiffuse()
= default;

void MeshObjectUnlitDiffuse::DrawObject(Matrix4f &transf, Matrix4f &projection) const
{
	// change program and reset if needed, bind buffers and set transform and projection
	DrawObjectCommonPre(transf, projection);

	const GLint hasDiffULocation = glGetUniformLocation(shader_program, "has_diffuse_map");
	assert(hasDiffULocation != -1);

	// set the active uniforms and texture units
	if (TObjectDiff != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TObjectDiff);
		glUniform1i(hasDiffULocation, 1);
	}
	else
	{
		glUniform1i(hasDiffULocation, 0);

		const GLint tintLoc = glGetUniformLocation(shader_program, "tint_color");
		assert(tintLoc != -1);
		glUniform3fv(tintLoc, 1, tintColor.get());
	}
	// Bind the diffuse texture in Texture Unit 0

	//Bind vert attribs and draw
	DrawObjectCommonPost();
}