#pragma once
#include <GL/glew.h>
#include "Vector3.h"

class MaterialObject
{
public:
	GLuint TObjectDiffuse0 = -1, TObjectNormal0 = -1, TObjectSpecular0 = -1;
	MaterialObject(const char* pathfilenameDiff, const char* pathfilenameNorm, const char* pathfilenameSpec);
	MaterialObject(const char* pathfilenameDiff);
	MaterialObject(const Vector3f& color);
	Vector3f tint;
	bool successfullyImported = true;
private:
	unsigned int TWidth = 0, THeight = 0;
	unsigned char *TextureDataDiffuse = nullptr, *TextureDataNorm = nullptr, *TextureDataSpec = nullptr;
	bool initTexture(const char * pathfilename, GLuint & TObject, unsigned char *& TextureData);
};
