#include "MeshObjectUnlitDiffuse.h"

MeshObjectUnlitDiffuse::MeshObjectUnlitDiffuse(const char * modelpath, MaterialObject& material_object, GLuint& sp) :
MeshObject(modelpath, sp),
TObjectDiff(material_object.TObjectDiffuse0)
{
	ImportModelAndSetVBOIBO(modelpath);
}

MeshObjectUnlitDiffuse::~MeshObjectUnlitDiffuse()
= default;

void MeshObjectUnlitDiffuse::DrawObject(Matrix4f &transf, Matrix4f &projection) const
{
	// change program and reset if needed, bind buffers and set transform and projection
	DrawObjectCommonPre(transf, projection);

	// set the active texture units

	// Bind the diffuse texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TObjectDiff);

	//Bind vert attribs and draw
	DrawObjectCommonPost();
}