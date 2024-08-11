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
		data = nullptr;
	}

	//Image(int height_, int width_, int channels_, Pixel** data_)
	//{
	//	this->height = height_;
	//	this->width = width_;
	//	this->channels = channels_;
	//	this->data = data_;
	//}

	Image(const Image& img)
	{
		this->height = img.height;
		this->width = img.width;
		this->channels = img.channels;
		this->data = new Pixel * [img.height];
		for (size_t y = 0; y < img.height; y++)
		{
			this->data[y] = new Pixel[img.width];
			for (size_t x = 0; x < img.width; x++)
			{
				data[y][x] = img.data[y][x];
			}
		}
	}
	Image(int height_, int width_, int channels_)
	{
		height = height_;
		width = width_;
		channels = channels_;
		data = new Pixel * [height];

		for (size_t y = 0; y < height; y++)
		{
			data[y] = new Pixel[width];
			for (size_t x = 0; x < width; x++)
			{
				data[y][x] = Pixel(channels);
			}
		}
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
				delete[] pixelData;
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

	void setDimensions(int width_, int height_)
	{
		this->width = width_;
		this->height = height_;
	}
	void setChannels(int channels_)
	{
		this->channels = channels_;
	}

	void getTexture(unsigned char** textureData) const
	{

		// delete[] * textureData;

		*textureData = new unsigned char[height * width * channels];

		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				for (size_t channel = 0; channel < channels; channel++)
				{
					int idx = (((y * width) + x) * channels) + channel;

					unsigned char value = this->data[y][x].getValue(channel);

					(*textureData)[idx] = value;
				}
			}
		}

	}
	unsigned char getMaxPixel() const
	{
		unsigned char maxVal = 0;

		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				Pixel grayPixel(data[y][x]);
				grayPixel.cvtGrayscale();
				if (grayPixel.getValue(0) > maxVal)
				{
					maxVal = grayPixel.getValue(0);
				}
			}
		}

		return maxVal;
	}
	PixelType getType() const
	{
		return data[0][0].getType();
	}
	int getWidth() const
	{
		return width;
	}
	int getHeight() const
	{
		return height;
	}
	int getChannels() const
	{
		return channels;
	}
	bool isNull() const
	{
		if (this->data == nullptr)
			return true;
		return false;
	}

	Pixel*& operator[](std::size_t idx) const
	{
		return this->data[idx];
	}

	Image& operator=(const Image& img)
	{
		deallocateImage();
		this->height = img.height;
		this->width = img.width;
		this->channels = img.channels;
		this->data = new Pixel * [img.height];
		for (size_t y = 0; y < img.height; y++)
		{
			this->data[y] = new Pixel[img.width];
			for (size_t x = 0; x < img.width; x++)
			{
				data[y][x] = img.data[y][x];
			}
		}
		return *this;
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