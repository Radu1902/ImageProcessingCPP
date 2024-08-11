#pragma once
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

enum class PixelType
{
	GRAY,
	RGB,
	HSV
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
	Pixel(const Pixel& px)
	{
		this->channels = px.channels;
		this->type = px.type;

		this->data = new unsigned char[channels];
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = px.getValue(i);
		}
	}
	Pixel(int channels_)
	{
		switch (channels_)
		{
		case 1:
			this->type = PixelType::GRAY;
			break;
		case 3:
			this->type = PixelType::RGB;
			break;
		default:
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

		switch (channels_)
		{
		case 1:
			this->type = PixelType::GRAY;
			break;
		case 3:
			this->type = PixelType::RGB;
			break;
		default:
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

	Pixel(PixelType type_)
	{
		if (type_ == PixelType::GRAY)
		{
			this->channels = 1;
		}
		else
		{
			this->channels = 3;
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
		if (type_ == PixelType::GRAY)
		{
			this->channels = 1;
		}
		else
		{
			this->channels = 3;
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
	unsigned char getValue(int channel) const
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
	PixelType getType() const
	{
		return type;
	}
	char normalize(int value)
	{
		if (value > 255)
			return 255;
		if (value < 0)
			return 0;
		return value;
	}

	void cvtGrayscale()
	{
		if (type == PixelType::GRAY)
			return;

		if (data == nullptr)
			return;

		short r = data[0];
		short g = data[1];
		short b = data[2];
		short result = r * 0.30 + g * 0.59 + b * 0.11;
		//short result = (r + b + g) / 3;
		result = normalize(result);

		delete[] data;

		this->channels = 1;
		this->type = PixelType::GRAY;
		this->data = new unsigned char[channels];
		this->data[0] = (char)result;
	}
	void cvtRGB()
	{
		if (type == PixelType::RGB || type == PixelType::HSV)
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
	void cvtHSV()
	{
		if (type == PixelType::HSV)
			return;
		if (type == PixelType::GRAY)
			cvtRGB();

		unsigned char r = data[0];
		unsigned char g = data[1];
		unsigned char b = data[2];

		unsigned char v = MAX(r, MAX(g, b));
		unsigned char s = (v - MIN(r, MIN(g, b))) / v;
		unsigned char h;

		if (r == g  && g == b)
		{
			h = 0;
		}
		else if (v == r && g >= b)
		{
			h = ((float)(g - b) / (float)(v - MIN(r, MIN(g, b))) * 60.0f) / 2;
		}
		else if (g == v)
		{
			h = ((float)(b - r) / (float)(v - MIN(r, MIN(g, b)) + 2) * 60.0f) / 2;
		}
		else if (b == v)
		{
			h = ((float)(r - g) / (float)(v - MIN(r, MIN(g, b)) + 4) * 60.0f) / 2;
		}
		else if (v == r && g < b)
		{
			h = ((float)(r - b) / (float)(v - MIN(r, MIN(g, b)) + 5) * 60.0f) / 2;
		}

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

	Pixel& operator=(const Pixel& px)
	{
		delete[] data;

		this->channels = px.channels;
		this->type = px.type;

		this->data = new unsigned char[channels];
		for (size_t i = 0; i < channels; i++)
		{
			data[i] = px.getValue(i);
		}
		return *this;
	}

	~Pixel()
	{
		delete[] data;
	}
};