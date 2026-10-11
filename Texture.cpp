#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Assigns the type of the texture to the texture object
	type = texType;

	// Stores the width, height, and the number of color channels of the image
	int widthImg = 0, heightImg = 0, numColCh = 0;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	if (!bytes)
	{
		std::string fallback = std::string("../") + image;
		bytes = stbi_load(fallback.c_str(), &widthImg, &heightImg, &numColCh, 0);
	}

	if (!bytes)
	{
		std::cout << "FAILED TO LOAD IMAGE: " << image << std::endl;
		std::cout << "Reason: " << stbi_failure_reason() << std::endl;
	}
	else
	{
		std::cout << "Image loaded successfully: " << image << std::endl;
		std::cout << "Width: " << widthImg << ", Height: " << heightImg << ", Channels: " << numColCh << std::endl;
	}

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum imgFormat = format;
	if (numColCh == 4)
		imgFormat = GL_RGBA;
	else if (numColCh == 3)
		imgFormat = GL_RGB;
	else if (numColCh == 1)
		imgFormat = GL_RED;

	GLint internalFormat = (imgFormat == GL_RGBA) ? GL_RGBA : ((imgFormat == GL_RED) ? GL_RED : GL_RGB);

	// Assigns the image to the OpenGL Texture object
	if (bytes)
	{
		glTexImage2D(texType, 0, internalFormat, widthImg, heightImg, 0, imgFormat, pixelType, bytes);
		// Generates MipMaps
		glGenerateMipmap(texType);
		// Deletes the image data as it is already in the OpenGL Texture object
		stbi_image_free(bytes);
	}

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}