#pragma once
#include "image_loader.h"
#include "imgui.h"
#include "Pixel.h"

class Image
{
	int height;
	int width;
	int channels;
	Pixel** data;

public:


	Image()
	{
		height = 0;
		width = 0;
		channels = 3;
		data = new Pixel*[0];
	}

	Image(int height_, int width_, int channels_, Pixel** data_)
	{
		deallocateImage();
		this->height = height_;
		this->width = width_;
		this->channels = channels_;
		this->data = data_;
	}

	Image(Image& img)
	{
		deallocateImage();
		this->height = img.height;
		this->width = img.width;
		this->channels = img.channels;
		this->data = img.data;
	}

	bool loadImage(const char* filepath, int image_channels)
	{
		// Load from file
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filepath, &image_width, &image_height, NULL, image_channels);
		if (image_data == NULL)
			return false;


		
		setImageMatrix(image_data, image_width, image_height, image_channels);

		stbi_image_free(image_data);
		return true;
	}

	void setImageMatrix(unsigned char* raw_data, int image_width, int image_height, int image_channels)
	{
		deallocateImage();
		this->width = image_width;
		this->height = image_height;
		this->channels = image_channels;

		this->data = new Pixel*[image_height];
		for (size_t y = 0; y < image_height; y++)
		{
			this->data[y] = new Pixel[image_width];

			for (size_t x = 0; x < image_width; x++)
			{
				int index = ((y * image_width) + x) * image_channels;

				unsigned char* pixelData = new unsigned char[image_channels];
				for (size_t channel = 0; channel < image_channels; channel++)
				{
					pixelData[channel] = raw_data[index + channel];
				}
				Pixel px(pixelData, image_channels);
				this->data[y][x] = px;
			}
		}
	}
	void deallocateImage()
	{
		for (size_t y = 0; y < height; y++)
		{
			delete[] this->data[y];
		}
		delete[] this->data;
	}


	Image convert2Gray()
	{
		if (data == nullptr)
		{
			Image img;
			return img;
		}
		Image grayscale(*this);

		for (size_t y = 0; y < this->height; y++)
		{
			for (size_t x = 0; x < this->width; x++)
			{
				grayscale[y][x].cvtGrayscale();
			}
		}

		return grayscale;
	}

	void print()
	{
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				data[y][x].print();
				printf(", ");
			}
		}
	}

	Pixel*& operator[](std::size_t idx) 
	{
		return this->data[idx];
	}

	~Image()
	{
		for (size_t y = 0; y < height; y++)
		{
			delete[] this->data[y];
		}
		delete[] this->data;
	}
};