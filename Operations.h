#pragma once
#include "Image.h"

void getHistogram(Image img, int channel, unsigned char** histogram)
{
	if (*histogram != nullptr)
	{
		delete[] *histogram;
	}

	*histogram = new unsigned char[256] ();
	for (size_t y = 0; y < img.getHeight(); y++)
	{
		for (size_t x = 0; x < img.getWidth(); x++)
		{
			(*histogram)[img[y][x].getValue(channel)]++;
		}
	}
}

Image convert2Gray(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale(input);

	for (size_t y = 0; y < input.getHeight(); y++)
	{
		for (size_t x = 0; x < input.getWidth(); x++)
		{
			grayscale[y][x].cvtGrayscale();
		}
	}
	grayscale.setChannels(1);

	return grayscale;
}
Image threshold(Image input, int thresh)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image threshed = convert2Gray(input);

	for (size_t y = 0; y < input.getHeight(); y++)
	{
		for (size_t x = 0; x < input.getWidth(); x++)
		{
			if (threshed[y][x].getValue(0) < thresh)
				threshed[y][x].setValue(0, 0);
			else
				threshed[y][x].setValue(255, 0);
		}
	}
	return threshed;
}
Image meanFilter(Image input, int ksize)
{
	if (ksize < 1 || ksize % 2 == 0 || ksize >= input.getWidth() || ksize >= input.getHeight())
	{
		Image img;
		return img;
	}

}



unsigned char* getOffsetLookUpTable(int offset)
{
	unsigned char* lut = new unsigned char[256];

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = pixelValue + offset;
		if (correspondingValue > 255)
			correspondingValue = 255;
		if (correspondingValue < 0)
			correspondingValue = 0;

		lut[pixelValue] = correspondingValue;
	}

	return lut;
}

Image modifyBrightess(Image input, int offset)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image output(input);

	unsigned char* lut = getOffsetLookUpTable(offset);

	for (size_t y = 0; y < output.getHeight(); y++)
	{
		for (size_t x = 0; x < output.getWidth(); x++)
		{
			output[y][x].setValue(lut[input[y][x].getValue(0)], 0);
		}
	}

	if (input.getType() == PixelType::RGB)
	{
		for (size_t y = 0; y < output.getHeight(); y++)
		{
			for (size_t x = 0; x < output.getWidth(); x++)
			{
				output[y][x].setValue(lut[input[y][x].getValue(1)], 1);
			}
		}
		for (size_t y = 0; y < output.getHeight(); y++)
		{
			for (size_t x = 0; x < output.getWidth(); x++)
			{
				output[y][x].setValue(lut[input[y][x].getValue(2)], 2);
			}
		}
	}
	delete[] lut;
	return output;
}