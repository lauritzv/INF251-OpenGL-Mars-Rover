#include "MaterialObject.h"
#include "lodepng.h"
#include <iostream>

MaterialObject::MaterialObject(const char* pathfilenameDiff, const char* pathfilenameNorm, const char* pathfilenameSpec, const Vector3f& specular_color) : specular(specular_color)
{
	successfullyImported =
		initTexture(pathfilenameDiff, TObjectDiffuse0,TextureDataDiffuse) &&
		initTexture(pathfilenameNorm, TObjectNormal0,TextureDataNorm) &&
		initTexture(pathfilenameSpec, TObjectSpecular0,TextureDataSpec);
}

MaterialObject::MaterialObject(const char* pathfilenameDiff)
{
	successfullyImported =
		initTexture(pathfilenameDiff, TObjectDiffuse0, TextureDataDiffuse);
}

MaterialObject::MaterialObject(const Vector3f& color) : tint(color)
{
}

MaterialObject::MaterialObject(const Vector3f& color, const Vector3f& specular_color) : tint(color), specular(specular_color)
{
}

MaterialObject::MaterialObject(const char* pathfilenameDiff, const Vector3f& specular_color) : specular(specular_color)
{
	successfullyImported =
		initTexture(pathfilenameDiff, TObjectDiffuse0, TextureDataDiffuse);
}

MaterialObject::MaterialObject(const char* pathfilenameDiff, const char* pathfilenameNorm, const Vector3f& specular_color)
{
	successfullyImported =
		initTexture(pathfilenameDiff, TObjectDiffuse0, TextureDataDiffuse) &&
		initTexture(pathfilenameNorm, TObjectNormal0, TextureDataNorm);
}

bool MaterialObject::initTexture(const char* pathfilename, GLuint &TObject, unsigned char* &TextureData)
{
	unsigned int fail = lodepng_decode_file(
		&TextureData, // the texture will be stored here
		&TWidth, &THeight, // width and height of the texture will be stored here
		pathfilename, // path and file name
		LCT_RGBA, // format of the image
		8); // bits for each color channel (bit depth / num. of channels)
	if (fail != 0)
	{
		std::cerr << "Error: cannot load texture: " << pathfilename << "." << std::endl;
		return false;
	}

	glGenTextures(1, &TObject); // Create the texture object
	glBindTexture(GL_TEXTURE_2D, TObject); // Bind it as a 2D texture

	// Set the texture data
	glTexImage2D(GL_TEXTURE_2D, // type of texture
		0,						// level of detail (used for mip-mapping only)
		GL_RGBA,				// color components (how the data should be interpreted)
		TWidth, THeight,		// texture width (must be a power of 2 on some systems)
		0,						// border thickness (just set this to 0)
		GL_RGBA,				// data format (how the data is supplied)
		GL_UNSIGNED_BYTE,		// the basic type of the data array
		TextureData);			// pointer to the data


	// Set texture parameters for minification and magnification

	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}
