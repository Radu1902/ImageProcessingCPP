#pragma once
#include "Image.h"

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