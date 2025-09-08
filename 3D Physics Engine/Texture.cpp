#include "Texture.h"

Texture::Texture(const char* imageName, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) : type(texType)
{
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(imageName, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes) {
		std::cout << "Failed to load texture";
	}
	std::cout << "Width: " << widthImg << ", Height: " << heightImg << ", Channels: " << numColCh << '\n';
	
	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);


	//When Minimizing the texture, apply GL_NEAREST
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//When Maximizing the texture, apply GL_NEAREST
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Repeat on both the S & T axis
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);


	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader shader, const char* uniform, GLuint unit)
{
	GLuint tex0UniID = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(tex0UniID, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::UnBind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
