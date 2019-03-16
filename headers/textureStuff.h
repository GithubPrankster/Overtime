#ifndef TEXHANDLER_H
#define TEXHANDLER_H

#include <glad/glad.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct texture {
public:
	unsigned int id;
	texture(const char* name, GLenum rgbType, GLenum filterSpace) {
		stbi_set_flip_vertically_on_load(true);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 4);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Near Filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterSpace);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterSpace);

		glGenerateMipmap(GL_TEXTURE_2D);
		glGenerateTextureMipmap(id);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, rgbType, width, height, 0, rgbType, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "The textureHandler did not appreciate this data screw-ery." << std::endl;
		}
		stbi_image_free(data);
	}

	void terminate() {
		glDeleteTextures(1, &id);
	}
};


#endif
//Leave as 0 if no texture.
void textureBind(unsigned int diffuse = 0, unsigned int specular = 0, unsigned int depth = 0) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depth);
}
