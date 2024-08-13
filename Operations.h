#pragma once
#include "Image.h"
#include "math.h"

// Basic operations

void getHistogram(Image img, int channel, unsigned int histogram[256])
{
	for (size_t y = 0; y < img.getHeight(); y++)
	{
		for (size_t x = 0; x < img.getWidth(); x++)
		{
			histogram[img[y][x].getValue(channel)]++;
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
	if (input.getType() == PixelType::GRAY)
	{
		return input;
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

Image convert2RGB(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	if (input.getType() == PixelType::RGB)
	{
		return input;
	}
	Image rgb(input);

	for (size_t y = 0; y < input.getHeight(); y++)
	{
		for (size_t x = 0; x < input.getWidth(); x++)
		{
			rgb[y][x].cvtRGB();
		}
	}
	rgb.setChannels(3);

	return rgb;
}

Image convert2HSV(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	if (input.getType() == PixelType::HSV)
	{
		return input;
	}
	Image hsv(input);

	for (size_t y = 0; y < input.getHeight(); y++)
	{
		for (size_t x = 0; x < input.getWidth(); x++)
		{
			hsv[y][x].cvtHSV();
		}
	}
	hsv.setChannels(3);

	return hsv;
}

Image invert(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}

	Image inverted(input);

	if (input.getType() == PixelType::GRAY)
	{
		for (size_t y = 0; y < input.getHeight(); y++)
		{
			for (size_t x = 0; x < input.getWidth(); x++)
			{
				inverted[y][x].setValue(255 - input[y][x].getValue(0), 0);
			}
		}

		return inverted;
	}

	for (size_t y = 0; y < input.getHeight(); y++)
	{
		for (size_t x = 0; x < input.getWidth(); x++)
		{
			inverted[y][x].setValue(255 - input[y][x].getValue(0), 0);
			inverted[y][x].setValue(255 - input[y][x].getValue(1), 1);
			inverted[y][x].setValue(255 - input[y][x].getValue(2), 2);
		}
	}

	return inverted;
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

Image applyLookUpTable(Image input, unsigned char lut[256])
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

void getOffsetLookUpTable(unsigned char lut[256], int offset)
{
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = pixelValue + offset;
		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image modifyBrightess(Image input, int offset)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getOffsetLookUpTable(lut, offset);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getFactoredLookUpTable(unsigned char lut[256], float factor)
{
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)(pixelValue * factor);
		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image modifyContrast(Image input, float factor)
{
	if (input.isNull() || factor < 0)
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getFactoredLookUpTable(lut, factor);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getLinearedLookUpTable(unsigned char lut[256], float factor, int offset)
{
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)(pixelValue * factor) + offset;
		lut[pixelValue] = pixelClamp(correspondingValue);

	}
}
Image modifyContrastAndBrightness(Image input, float factor, int offset)
{
	if (input.isNull() || factor < 0)
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getLinearedLookUpTable(lut, factor, offset);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getLogarithmicLookUpTable(unsigned char lut[256], unsigned char maxVal)
{
	float scalingConstant = 255 / log(1 + maxVal);

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)(scalingConstant * log(1 + pixelValue));
		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image logOperator(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char maxVal = input.getMaxPixel();
	unsigned char lut[256] = { 0 };
	getLogarithmicLookUpTable(lut, maxVal);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getGammaLookUpTable(unsigned char lut[256], float gamma)
{
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		float correspondingValue = (float)pixelValue / 255.0f;

		correspondingValue = pow(correspondingValue, gamma);

		correspondingValue = (int)(correspondingValue * 255);
		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image gammaOperator(Image input, float gamma)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getGammaLookUpTable(lut, gamma);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getPiecewiseLookUpTable(unsigned char lut[256], unsigned char r1, unsigned char s1, unsigned char r2, unsigned char s2)
{
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
}
Image piecewiseLinearContrast(Image input, int r1, int s1, int r2, int s2)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getPiecewiseLookUpTable(lut, r1, s1, r2, s2);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getEMLookUpTable(unsigned char lut[256], float e, int m)
{
	float c = (255 - (255 * pow(255, e) / (pow(255, e) + pow(255, e)))) / 65025;

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		float correspondingValue = 255 * (pow(pixelValue, e) / (pow(pixelValue, e) + pow(m, e)) + c * pixelValue);

		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image emOperator(Image input, float e, int m)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getEMLookUpTable(lut, e, m);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getSplineLookUpTable(unsigned char lut[256], int* splinePointsX, int* splinePointsY)
{

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		float correspondingValue = 0;

		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image splineOperator(Image input, int* splinePointsX, int* splinePointsY)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char lut[256] = { 0 };
	getSplineLookUpTable(lut, splinePointsX, splinePointsY);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getStretchedLookUpTable(unsigned char lut[256], unsigned char minPixel, unsigned char maxPixel)
{
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		int correspondingValue = (int)((255.0f / (float)(maxPixel - minPixel)) * (float)(pixelValue - minPixel));

		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image histogramStretchingOperator(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	unsigned char minPixel = input.getMinPixel();
	unsigned char maxPixel = input.getMaxPixel();
	unsigned char lut[256] = { 0 };
	getStretchedLookUpTable(lut, minPixel, maxPixel);
	Image output = applyLookUpTable(input, lut);
	return output;
}

void getEqualizedLookUpTable(unsigned char lut[256], unsigned int histogram[256], int pixelCount)
{
	unsigned int cumulation = 0;
	float normalizedCumulation = 0.0f;
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		cumulation += histogram[pixelValue];
		normalizedCumulation = (float)cumulation / (float)pixelCount * 255.0f;
		lut[pixelValue] = (char)normalizedCumulation;
	}
}
Image histogramEqualizationOperator(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image hsvInput = convert2HSV(input);
	unsigned int valueHistogram[256] = { 0 };
	getHistogram(hsvInput, 2, valueHistogram);
	unsigned char lut[256] = { 0 };
	getEqualizedLookUpTable(lut, valueHistogram, input.getHeight() * input.getWidth());
	Image output = applyLookUpTable(input, lut);
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

	for (size_t y = 0; y < threshed.getHeight(); y++)
	{
		for (size_t x = 0; x < threshed.getWidth(); x++)
		{
			if (threshed[y][x].getValue(0) < thresh)
				threshed[y][x].setValue(0, 0);
			else
				threshed[y][x].setValue(255, 0);
		}
	}
	return threshed;
}

Image colorThreshold(Image input, unsigned char minR, unsigned char minG, unsigned char minB, unsigned char maxR, unsigned char maxG, unsigned char maxB)
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
			if (input[y][x].getValue(0) > minR && input[y][x].getValue(0) < maxR && input[y][x].getValue(1) > minG && input[y][x].getValue(1) < maxG && input[y][x].getValue(2) > minB && input[y][x].getValue(2) < maxB)
				threshed[y][x].setValue(255, 0);
			else
				threshed[y][x].setValue(0, 0);
		}
	}
	return threshed;
}

Image dualThreshold(Image input, unsigned char thresh1, unsigned char thresh2)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}

	Image threshed = convert2Gray(input);

	for (size_t y = 0; y < threshed.getHeight(); y++)
	{
		for (size_t x = 0; x < threshed.getWidth(); x++)
		{
			if (threshed[y][x].getValue(0) < thresh1)
				threshed[y][x].setValue(0, 0);
			else if (threshed[y][x].getValue(0) < thresh2)
				threshed[y][x].setValue(128, 0);
			else
				threshed[y][x].setValue(255, 0);
		}
	}
	return threshed;
}

Image midRangeThreshold(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);

	unsigned char minPixel = grayscale.getMinPixel();
	unsigned char maxPixel = grayscale.getMaxPixel();

	unsigned char midThresh = (unsigned char)(((float)minPixel + (float)maxPixel) / 2.0f);

	return threshold(grayscale, midThresh);
}

Image meanThreshold(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);
	unsigned int histogram[256] = { 0 };
	getHistogram(grayscale, 0, histogram);

	unsigned int histSum = 0;
	float weightedSum = 0;

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		histSum += histogram[pixelValue];
		weightedSum += histogram[pixelValue] * pixelValue;
	}
	unsigned char averagePixelValue = (unsigned char)(weightedSum / (float)histSum);

	return threshold(grayscale, averagePixelValue);
}

Image quantileThreshold(Image input, float backgroundPercentage)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);
	
	unsigned int histogram[256] = { 0 };
	getHistogram(grayscale, 0, histogram);

	unsigned int pixelCount = input.getHeight() * input.getWidth();
	unsigned int possibleThresh = 0;
	unsigned int pixelSum = 0;

	for (possibleThresh = 0; possibleThresh < 256; possibleThresh++)
	{
		pixelSum = 0;
		for (size_t k = 0; k < possibleThresh; k++)
		{
			pixelSum += histogram[k];
		}
		if (pixelSum >= pixelCount * backgroundPercentage)
			break;
	}
	return threshold(grayscale, (unsigned char)possibleThresh);
}

Image intermeansThreshold(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);

	unsigned int histogram[256] = { 0 };
	getHistogram(grayscale, 0, histogram);

	unsigned long long histSum = 0;
	float weightedSum = 0;

	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		histSum += histogram[pixelValue];
		weightedSum += histogram[pixelValue] * pixelValue;
	}

	unsigned int possibleThresh = (unsigned char)(weightedSum / (float)histSum);
	unsigned int previousThresh = 0;
	int threshDifference = possibleThresh - previousThresh;

	while (threshDifference > 5 || threshDifference < -5)
	{
		int class1HistSum = 0;
		int class2HistSum = 0;
		float class1WeightedSum = 0.0f;
		float class2WeightedSum = 0.0f;

		for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
		{
			if (pixelValue < possibleThresh)
			{
				class1HistSum += histogram[pixelValue];
				class1WeightedSum += histogram[pixelValue] * pixelValue;
			}
			else
			{
				class2HistSum += histogram[pixelValue];
				class2WeightedSum += histogram[pixelValue] * pixelValue;
			}
		}
		float class1Mean = class1WeightedSum / (float)class1HistSum;
		float class2Mean = class2WeightedSum / (float)class2HistSum;

		previousThresh = possibleThresh;
		possibleThresh = (unsigned int)((class1Mean + class2Mean) / 2.0f);
		threshDifference = possibleThresh - previousThresh;
	}
	return threshold(grayscale, (unsigned char)possibleThresh);
}

Image otsuThreshold(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);

	unsigned int histogram[256] = { 0 };
	getHistogram(grayscale, 0, histogram);

	unsigned int pixelCount = input.getHeight() * input.getWidth();
	unsigned int optimalThreshold = 0;

	double maxVariance = 0;
	double optimalThresholdSum = 0;
	int optimalThresholdCount = 0;
	for (unsigned int possibleThresh = 0; possibleThresh < 255; possibleThresh++)
	{
		double class1Probability = 0;
		double class1Mean = 0;
		for (size_t pixelValue = 0; pixelValue < possibleThresh + 1; pixelValue++)
		{
			double pixelProbability = histogram[pixelValue] / (double)pixelCount;
			class1Probability += pixelProbability;
			class1Mean += pixelValue * pixelProbability;
		}
		class1Mean /= class1Probability;
		double class1Variance = 0;
		for (size_t pixelValue = 0; pixelValue < possibleThresh + 1; pixelValue++)
		{
			class1Variance += pow((pixelValue - class1Mean), 2) * histogram[pixelValue] / (double)pixelCount;
		}
		class1Variance /= class1Probability;


		double class2Probability = 0;
		double class2Mean = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			double pixelProbability = histogram[pixelValue] / (double)pixelCount;
			class2Probability += pixelProbability;
			class2Mean += pixelValue * pixelProbability;
		}
		class2Mean /= class2Probability;
		double class2Variance = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			class2Variance += pow((pixelValue - class2Mean), 2) * histogram[pixelValue] / (double)pixelCount;
		}
		class2Variance /= class2Probability;

		double mean = 1.0f / (double)pixelCount;
		double betweenClassVariance = class1Probability * pow((class1Mean - mean), 2) + class2Probability * pow((class2Mean - mean), 2);

		if (betweenClassVariance > maxVariance)
		{
			maxVariance = betweenClassVariance;
			optimalThreshold = possibleThresh;
			optimalThresholdSum = possibleThresh;
			optimalThresholdCount = 1;
		}
		if (betweenClassVariance == maxVariance)
		{
			optimalThresholdSum += possibleThresh;
			optimalThresholdCount++;
		}
	}
	optimalThreshold = optimalThresholdSum / (float)optimalThresholdCount;
	return threshold(grayscale, (unsigned char)optimalThreshold);
}
Image otsuDualThreshold(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);

	unsigned int histogram[256] = { 0 };
	getHistogram(grayscale, 0, histogram);

	unsigned int pixelCount = input.getHeight() * input.getWidth();
	unsigned int optimalThreshold = 0;

	double maxVariance = 0;
	double optimalThresholdSum = 0;
	int optimalThresholdCount = 0;
	for (unsigned int possibleThresh = 0; possibleThresh < 255; possibleThresh++)
	{
		double class1Probability = 0;
		double class1Mean = 0;
		for (size_t pixelValue = 0; pixelValue < possibleThresh + 1; pixelValue++)
		{
			double pixelProbability = histogram[pixelValue] / (double)pixelCount;
			class1Probability += pixelProbability;
			class1Mean += pixelValue * pixelProbability;
		}
		class1Mean /= class1Probability;
		double class1Variance = 0;
		for (size_t pixelValue = 0; pixelValue < possibleThresh + 1; pixelValue++)
		{
			class1Variance += pow((pixelValue - class1Mean), 2) * histogram[pixelValue] / (double)pixelCount;
		}
		class1Variance /= class1Probability;


		double class2Probability = 0;
		double class2Mean = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			double pixelProbability = histogram[pixelValue] / (double)pixelCount;
			class2Probability += pixelProbability;
			class2Mean += pixelValue * pixelProbability;
		}
		class2Mean /= class2Probability;
		double class2Variance = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			class2Variance += pow((pixelValue - class2Mean), 2) * histogram[pixelValue] / (double)pixelCount;
		}
		class2Variance /= class2Probability;

		double mean = 1.0f / (double)pixelCount;
		double betweenClassVariance = class1Probability * pow((class1Mean - mean), 2) + class2Probability * pow((class2Mean - mean), 2);

		if (betweenClassVariance > maxVariance)
		{
			maxVariance = betweenClassVariance;
			optimalThreshold = possibleThresh;
			optimalThresholdSum = possibleThresh;
			optimalThresholdCount = 1;
		}
		if (betweenClassVariance == maxVariance)
		{
			optimalThresholdSum += possibleThresh;
			optimalThresholdCount++;
		}
	}
	optimalThreshold = optimalThresholdSum / (float)optimalThresholdCount;
	return threshold(grayscale, (unsigned char)optimalThreshold);
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