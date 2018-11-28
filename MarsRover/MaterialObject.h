#pragma once
#include <GL/glew.h>
#include "Vector3.h"

class MaterialObject
{
public:
	GLuint TObjectDiffuse0 = -1, TObjectNormal0 = -1, TObjectSpecular0 = -1;
	MaterialObject(const char* pathfilenameDiff, const char* pathfilenameNorm, const char* pathfilenameSpec, const Vector3f& specular_color);
	MaterialObject(const char* pathfilenameDiff);
	MaterialObject(const Vector3f& color);

	MaterialObject(const Vector3f& color, const Vector3f& specular_color);
	MaterialObject(const char* pathfilenameDiff, const Vector3f& specular_color);
	MaterialObject(const char* pathfilenameDiff, const char* pathfilenameNorm, const Vector3f& specular_color);

	Vector3f tint = Vector3f(1.f,1.f,1.f);
	Vector3f specular = Vector3f(2.f,2.f,2.f);
	bool successfullyImported = true;
private:
	unsigned int TWidth = 0, THeight = 0;
	unsigned char *TextureDataDiffuse = nullptr, *TextureDataNorm = nullptr, *TextureDataSpec = nullptr;
	bool initTexture(const char * pathfilename, GLuint & TObject, unsigned char *& TextureData);
};
