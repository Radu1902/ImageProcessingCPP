#pragma once
#include "image_loader.h"
#include "imgui.h"


class Image
{
	int height;
	int width;
	int channels;
	unsigned char** data;

public:


	Image()
	{
		height = 0;
		width = 0;
		channels = 0;
		data = nullptr;
	}

	Image(int height_, int width_, int channels_, unsigned char** data_)
	{
		this->height = height_;
		this->width = width_;
		this->channels = channels_;
		this->data = data_;
	}

	bool loadImage(const char* filepath, int image_channels)
	{
		// Load from file
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filepath, &image_width, &image_height, NULL, image_channels);
		if (image_data == NULL)
			return false;


		this->width = image_width;
		this->height = image_height;
		this->channels = image_channels;
		setImageMatrix(image_data, image_width, image_height, image_channels);

		stbi_image_free(image_data);
		return true;
	}

	unsigned char** setImageMatrix(unsigned char* raw_data, int image_width, int image_height, int image_channels)
	{
		unsigned char** matrix_data = new unsigned char*[image_height];
		for (size_t i = 0; i < image_height; i++)
		{
			unsigned char* matrix_row = new unsigned char[image_width];
			matrix_data[i] = matrix_row;

			for (size_t j = 0; j < image_width; j++)
			{

			}
		}
	}
};