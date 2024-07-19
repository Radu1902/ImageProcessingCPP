#pragma once

enum class PixelType
{
	GRAY,
	RGB,
	RGBA
};

class Pixel
{
	unsigned char* data;
	int channels;
	PixelType type;

public:
	Pixel()
	{
		this->channels = 3;
		this->data = new unsigned char[channels];
		this->type = PixelType::RGB;
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = 0;
		}
	}
	Pixel(unsigned char* data_, int channels_, PixelType type_)
	{
		this->data = data_;
		this->channels = channels_;
		this->type = type_;
	}
	Pixel(int channels_)
	{

		if (channels_ == 1)
			this->type = PixelType::GRAY;
		else if (channels_ == 3)
			this->type = PixelType::RGB;
		else if (channels_ == 4)
			this->type = PixelType::RGBA;
		else
		{
			this->channels = 3;
			this->data = this->data = new unsigned char[channels];
			this->type = PixelType::RGB;
			for (size_t i = 0; i < channels; i++)
			{
				data[i] = 0;
			}
			return;
		}

		this->data = new unsigned char[channels_];
		this->channels = channels_;
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = 0;
		}
	}
	Pixel(PixelType type_)
	{
		switch (type)
		{
		case PixelType::GRAY:
			this->channels = 1;
			break;
		case PixelType::RGB:
			this->channels = 3;
			break;
		case PixelType::RGBA:
			this->channels = 4;
			break;
		default:
			this->channels = 3;
			this->data = new unsigned char[channels];
			this->type = PixelType::RGB;
			for (size_t i = 0; i < channels; i++)
			{
				data[i] = 0;
			}
			return;
		}
		this->type = type_;
		this->data = new unsigned char[channels];
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = 0;
		}
	}
	Pixel(unsigned char* data_, PixelType type_)
	{
		switch (type)
		{
		case PixelType::GRAY:
			this->channels = 1;
			break;
		case PixelType::RGB:
			this->channels = 3;
			break;
		case PixelType::RGBA:
			this->channels = 4;
			break;
		default:
			this->channels = 3;
			this->data = new unsigned char[channels];
			this->type = PixelType::RGB;
			for (size_t i = 0; i < channels; i++)
			{
				data[i] = 0;
			}
			return;
		}

		this->type = type_;


		this->data = new unsigned char[channels];
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = data_[i];
		}
	}

	void setValue(unsigned char value, int channel)
	{
		if (channel < 0 || channel >= this->channels)
			return;
		
		this->data[channel] = value;
	}

	void cvtGrayscale()
	{
		if (channels < 3)
			return;

		if (data == nullptr)
			return;

		char r = data[0];
		char g = data[1];
		char b = data[2];

		delete[] this->data;

		this->channels = 1;
		this->type = PixelType::GRAY;
		this->data = new unsigned char[channels];
		this->data[0] = (r + b + g) / 3;

	}
	
};