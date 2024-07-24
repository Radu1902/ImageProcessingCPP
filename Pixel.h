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
	//Pixel(unsigned char* data_, int channels_, PixelType type_)
	//{
	//	this->data = data_;
	//	this->channels = channels_;
	//	this->type = type_;
	//}
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

		this->channels = channels_;
		this->data = new unsigned char[channels];
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = 0;
		}
	}
	Pixel(unsigned char* data_, int channels_)
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

		this->channels = channels_;
		this->data = new unsigned char[channels];
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = data_[i];
		}
	}

	//Pixel(PixelType type_)
	//{
	//	switch (type)
	//	{
	//	case PixelType::GRAY:
	//		this->channels = 1;
	//		break;
	//	case PixelType::RGB:
	//		this->channels = 3;
	//		break;
	//	case PixelType::RGBA:
	//		this->channels = 4;
	//		break;
	//	default:
	//		this->channels = 3;
	//		this->data = new unsigned char[channels];
	//		this->type = PixelType::RGB;
	//		for (size_t i = 0; i < channels; i++)
	//		{
	//			data[i] = 0;
	//		}
	//		return;
	//	}
	//	this->type = type_;
	//	this->data = new unsigned char[channels];
	//	for (size_t i = 0; i < channels; i++)
	//	{
	//		data[i] = 0;
	//	}
	//}
	//Pixel(unsigned char* data_, PixelType type_)
	//{
	//	switch (type)
	//	{
	//	case PixelType::GRAY:
	//		this->channels = 1;
	//		break;
	//	case PixelType::RGB:
	//		this->channels = 3;
	//		break;
	//	case PixelType::RGBA:
	//		this->channels = 4;
	//		break;
	//	default:
	//		this->channels = 3;
	//		this->data = new unsigned char[channels];
	//		this->type = PixelType::RGB;
	//		for (size_t i = 0; i < channels; i++)
	//		{
	//			data[i] = 0;
	//		}
	//		return;
	//	}

	//	this->type = type_;


	//	this->data = new unsigned char[channels];
	//	for (size_t i = 0; i < channels; i++)
	//	{
	//		data[i] = data_[i];
	//	}
	//}

	void setValue(unsigned char value, int channel)
	{
		if (channel < 0 || channel >= this->channels)
			return;
		
		this->data[channel] = value;
	}
	unsigned char getValue(int channel)
	{
		try
		{
			if (channel < 0 || channel >= this->channels)
				throw (channel);
			return data[channel];
		}
		catch (int channel) 
		{
			printf("invalid channel, %d", channel);
		}
	}
	short normalize(short value)
	{
		if (value > 255)
			return 255;
		if (value < 0)
			return 0;
		return value;
	}

	void cvtGrayscale()
	{
		if (channels < 3)
			return;

		if (data == nullptr)
			return;

		short r = data[0];
		short g = data[1];
		short b = data[2];
		short result = r * 0.30 + g * 0.59 + b * 0.11;
		//short result = (r + b + g) / 3;
		result = normalize(result);

		delete[] this->data;

		this->channels = 1;
		this->type = PixelType::GRAY;
		this->data = new unsigned char[channels];
		this->data[0] = (char)result;
	}
	void cvtGrayscaleAlt()
	{
		if (channels < 3)
			return;

		if (data == nullptr)
			return;

		short r = data[0];
		short g = data[1];
		short b = data[2];
		short result = (r + b + g) / 3;
		result = normalize(result);

		delete[] this->data;

		this->channels = 1;
		this->type = PixelType::GRAY;
		this->data = new unsigned char[channels];
		this->data[0] = (char)result;
	}
	void cvtColor()
	{
		if (channels == 1)
			return;

		if (data == nullptr)
			return;

		char value = data[0];

		delete[] this->data;

		this->channels = 3;
		this->type = PixelType::RGB;
		this->data = new unsigned char[channels];
		this->data[0] = value;
		this->data[1] = value;
		this->data[2] = value;

	}
	void print()
	{
		printf("[");
		for (size_t i = 0; i < channels; i++)
		{
			printf("%d ", data[i]);
		}
		printf("]");
	}
	unsigned char* getData()
	{
		return this->data;
	}

	//~Pixel()
	//{
	//	delete[] data;
	//}
};