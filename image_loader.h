#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers


void printImgData(unsigned char* img, int height, int width, int channels)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = ((y * width) + x) * channels;
            unsigned int r = img[index];
            unsigned int g = img[index + 1];
            unsigned int b = img[index + 2];

            printf("R:%d G:%d B:%d  ", r, g, b);
        }
        printf("\n");
    }
}
// Simple helper function to load an image into a OpenGL texture with common settings
bool loadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    int image_channels = 3;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, image_channels);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);

    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

bool writeTextureToFile(const char* filename, int width, int height, int channels, unsigned char* texture)
{
    return stbi_write_bmp(filename, width, height, channels, texture);
}