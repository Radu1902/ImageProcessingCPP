#pragma once
#include "Image.h"
#include "math.h"

// Basic operations

void getHistogram(Image img, int channel, unsigned int** histogram)
{
	if (*histogram != nullptr)
	{
		delete[] *histogram;
	}

	*histogram = new unsigned int[256] ();
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

// pointwise operations

unsigned char pixelClamp(int value)
{
	if (value > 255)
		return 255;
	if (value < 0)
		return 0;
	return value;
}

Image applyLookUpTable(Image input, unsigned char* lut)
{
	Image output(input);

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
	return output;
}

unsigned char* getOffsetLookUpTable(int offset)
{
	unsigned char* lut = new unsigned char[256];

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = pixelValue + offset;
		lut[pixelValue] = pixelClamp(correspondingValue);

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
	unsigned char* lut = getOffsetLookUpTable(offset);
	Image output = applyLookUpTable(input, lut);
	delete[] lut;
	return output;
}

unsigned char* getFactoredLookUpTable(float factor)
{
	unsigned char* lut = new unsigned char[256];

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)(pixelValue * factor);
		lut[pixelValue] = pixelClamp(correspondingValue);

	}

	return lut;
}
Image modifyContrast(Image input, float factor)
{
	if (input.isNull() || factor < 0)
	{
		Image img;
		return img;
	}
	unsigned char* lut = getFactoredLookUpTable(factor);
	Image output = applyLookUpTable(input, lut);
	delete[] lut;
	return output;
}

unsigned char* getLinearedLookUpTable(float factor, int offset)
{
	unsigned char* lut = new unsigned char[256];

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)(pixelValue * factor) + offset;
		lut[pixelValue] = pixelClamp(correspondingValue);

	}

	return lut;
}
Image modifyContrastAndBrightness(Image input, float factor, int offset)
{
	if (input.isNull() || factor < 0)
	{
		Image img;
		return img;
	}
	unsigned char* lut = getLinearedLookUpTable(factor, offset);
	Image output = applyLookUpTable(input, lut);
	delete[] lut;
	return output;
}

unsigned char* getLogarithmicLookUpTable(unsigned char maxVal)
{
	unsigned char* lut = new unsigned char[256];

	float scalingConstant = 255 / log(1 + maxVal);

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)(scalingConstant * log(1 + pixelValue));
		lut[pixelValue] = pixelClamp(correspondingValue);
	}

	return lut;
}
Image logOperator(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char maxVal = input.getMaxPixel();
	unsigned char* lut = getLogarithmicLookUpTable(maxVal);
	Image output = applyLookUpTable(input, lut);
	delete[] lut;
	return output;
}

unsigned char* getGammaLookUpTable(float gamma)
{
	unsigned char* lut = new unsigned char[256];

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		float correspondingValue = (float)pixelValue / 255.0f;

		correspondingValue = pow(correspondingValue, gamma);

		correspondingValue = (int)(correspondingValue * 255);
		lut[pixelValue] = pixelClamp(correspondingValue);

	}

	return lut;
}
Image gammaOperator(Image input, float gamma)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char* lut = getGammaLookUpTable(gamma);
	Image output = applyLookUpTable(input, lut);
	delete[] lut;
	return output;
}

unsigned char* getPiecewiseLookUpTable(unsigned char r1, unsigned char s1, unsigned char r2, unsigned char s2)
{
	unsigned char* lut = new unsigned char[256];

	float interval1Factor = (float)s1 / (float)r1;
	for (size_t pixelValue = 0; pixelValue < r1; pixelValue++)
	{
		int correspondingValue = pixelValue * interval1Factor;
		lut[pixelValue] = pixelClamp(correspondingValue);

	}
	float interval2Factor = (s2 - s1) / (r2 - r1);
	int interval2Offset = s1;
	for (size_t pixelValue = r1; pixelValue < r2; pixelValue++)
	{
		int correspondingValue = (interval2Factor * (pixelValue - r1)) + interval2Offset;
		lut[pixelValue] = pixelClamp(correspondingValue);

	}
	float interval3Factor = (255 - s2) / (255 - r2);
	int interval3Offset = s2;
	for (size_t pixelValue = r2; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (interval3Factor * (pixelValue - r2)) + interval3Offset;
		lut[pixelValue] = pixelClamp(correspondingValue);

	}

	return lut;
}
Image piecewiseLinearContrast(Image input, int r1, int s1, int r2, int s2)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char* lut = getPiecewiseLookUpTable(r1, s1, r2, s2);
	Image output = applyLookUpTable(input, lut);
	delete[] lut;
	return output;
}

// thresholdings

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

// filters

Image meanFilter(Image input, int ksize)
{
	if (ksize < 1 || ksize % 2 == 0 || ksize >= input.getWidth() || ksize >= input.getHeight())
	{
		Image img;
		return img;
	}

}