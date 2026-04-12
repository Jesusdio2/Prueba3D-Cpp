#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "splash_texture.h"
#include <GLES3/gl3.h>
#include <iostream>

bool SplashTexture::Load(const std::string& path) {

    int channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (!data) {
        std::cout << "Error cargando splash: " << path << std::endl;
        return false;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);

    std::cout << "Splash cargado: " << width << "x" << height << std::endl;
    return true;
}

void SplashTexture::Destroy() {
    if (id != 0) {
        glDeleteTextures(1, &id);
        id = 0;
    }
}