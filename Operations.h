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
	// out of order
	for (size_t pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		float correspondingValue = 0;

		lut[pixelValue] = pixelClamp(correspondingValue);
	}
}
Image splineOperator(Image input, int* splinePointsX, int* splinePointsY)
{
	// out of order
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

Image doubleThreshold(Image input, unsigned char thresh1, unsigned char thresh2)
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

		double class2Probability = 1 - class1Probability;
		double class2Mean = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			double pixelProbability = histogram[pixelValue] / (double)pixelCount;
			class2Mean += pixelValue * pixelProbability;
		}
		class2Mean /= class2Probability;

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

Image minErrorThreshold(Image input)
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
	unsigned int minThreshold = 0;
	double minError = DBL_MAX;

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
			double pixelProbability = (double)histogram[pixelValue] / (double)pixelCount;
			class1Variance += pow((pixelValue - class1Mean), 2) * pixelProbability;
		}
		class1Variance /= class1Probability;

		double class2Probability = 1.0 - class1Probability;
		double class2Mean = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			double pixelProbability = histogram[pixelValue] / (double)pixelCount;
			class2Mean += pixelValue * pixelProbability;
		}
		class2Mean /= class2Probability;
		double class2Variance = 0;
		for (size_t pixelValue = possibleThresh + 1; pixelValue < 256; pixelValue++)
		{
			double pixelProbability = (double)histogram[pixelValue] / (double)pixelCount;
			class2Variance += pow((pixelValue - class2Mean), 2) * pixelProbability;
		}
		class2Variance /= class2Probability;

		double err = 1.0 + class1Probability * log(class1Variance) + class2Probability * log(class2Variance) - 2 * class1Probability * log(class1Probability) - 2 * class2Probability * log(class2Probability);
		
		if (err < minError && err >= 0)
		{
			minError = err;
			minThreshold = possibleThresh;
		}
	}
	return threshold(grayscale, (unsigned char)minThreshold);
}

Image otsuDoubleThreshold(Image input)
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
	unsigned int optimalLow = 0;
	unsigned int optimalHigh = 1;

	double maxVariance = 0;
	double optimalHighSum = 0;
	double optimalLowSum = 0;
	int optimalThresholdCount = 0;
	for (unsigned int possibleLow = 0; possibleLow < 254; possibleLow++)
	{
		for (unsigned int possibleHigh = possibleLow + 1; possibleHigh < 255; possibleHigh++)
		{
			// class 1

			double class1Probability = 0;
			double class1Mean = 0;
			for (size_t pixelValue = 0; pixelValue < possibleLow + 1; pixelValue++)
			{
				double pixelProbability = histogram[pixelValue] / (double)pixelCount;
				class1Probability += pixelProbability;
				class1Mean += pixelValue * pixelProbability;
			}
			class1Mean /= class1Probability;

			// class 2

			double class2Probability = 0;
			double class2Mean = 0;
			for (size_t pixelValue = possibleLow + 1; pixelValue < possibleHigh; pixelValue++)
			{
				double pixelProbability = histogram[pixelValue] / (double)pixelCount;
				class2Probability += pixelProbability;
				class2Mean += pixelValue * pixelProbability;
			}
			class2Mean /= class2Probability;

			// class 3

			double class3Probability = 0;
			double class3Mean = 0;
			for (size_t pixelValue = possibleHigh + 1; pixelValue < 256; pixelValue++)
			{
				double pixelProbability = histogram[pixelValue] / (double)pixelCount;
				class3Probability += pixelProbability;
				class3Mean += pixelValue * pixelProbability;
			}
			class3Mean /= class3Probability;

			double mean = 1.0f / (double)pixelCount;
			double betweenClassVariance = class1Probability * pow((class1Mean - mean), 2) + class2Probability * pow((class2Mean - mean), 2) + class3Probability * pow((class3Mean - mean), 2);

			if (betweenClassVariance > maxVariance)
			{
				maxVariance = betweenClassVariance;
				optimalLow = possibleLow;
				optimalHigh = possibleHigh;
				optimalLowSum = possibleLow;
				optimalHighSum = possibleHigh;
				optimalThresholdCount = 1;
			}
			if (betweenClassVariance == maxVariance)
			{
				optimalLowSum += possibleLow;
				optimalHighSum += possibleHigh;
				optimalThresholdCount++;
			}
		}
	}
	optimalLow = optimalLowSum / (float)optimalThresholdCount;
	optimalHigh = optimalHighSum / (float)optimalThresholdCount;
	return doubleThreshold(grayscale, (unsigned char)optimalLow, (unsigned char)optimalHigh);
}


void getSummationTerms(unsigned int histogram[256], unsigned int pixelCount, double summationTerm1[256], double summationTerm2[256])
{
	// assumes elements summationTerm0 and summationTerm1 are all initialized to 0

	double termValue = 0;
	for (unsigned int pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		double pixelProbability = histogram[pixelValue] / pixelCount;
		if (pixelProbability > 0)
		{
			termValue += pixelProbability * log(pixelProbability);
		}
		summationTerm1[pixelValue] = termValue;
	}

	termValue = 0;
	for (int pixelValue = 255; pixelValue >= 0; pixelValue--)
	{
		double pixelProbability = histogram[pixelValue] / pixelCount;
		summationTerm2[pixelValue] = termValue;
		if (pixelProbability > 0)
		{
			termValue += pixelProbability * log(pixelProbability);
		}
	}
}
Image maxEntropyThresholding(Image input)
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
	double summationTerm1[256] = { 0 };
	double summationTerm2[256] = { 0 };
	getSummationTerms(histogram, pixelCount, summationTerm1, summationTerm2);

	unsigned int maxThresh = 0;
	double maxEntropy = -999999.0;

	for (unsigned int possibleThresh = 0; possibleThresh < 255; possibleThresh++)
	{
		double class1Probability = 0;
		for (size_t pixelValue = 0; pixelValue < possibleThresh + 1; pixelValue++)
			class1Probability += histogram[pixelValue] / (double)pixelCount;

		double class2Probability = 1.0 - class1Probability;

		double backgroundEntropy = 0;
		double foregroundEntropy = 0;
		if (class1Probability > 0)
			backgroundEntropy = -1.0 / class1Probability * summationTerm1[possibleThresh] + log(class1Probability);

		if (class2Probability > 0)
			foregroundEntropy = -1.0 / class2Probability * summationTerm2[possibleThresh] + log(class2Probability);

		double entropy = backgroundEntropy + foregroundEntropy;

		if (entropy > maxEntropy && entropy < 0.0)
		{
			maxEntropy = entropy;
			maxThresh = possibleThresh;
		}
	}
	return threshold(grayscale, (unsigned char)maxThresh);
}

Image triangleThresholding(Image input)
{
	if (input.isNull())
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);

	unsigned int histogram[256] = { 0 };
	getHistogram(grayscale, 0, histogram);

	unsigned char peak = 0; // most common pixel value is called "peak" (as in the peak of the histogram)
	unsigned int peakValue = 0;
	for (unsigned int pixelValue = 0; pixelValue < 256; pixelValue++)
	{
		if (histogram[pixelValue] > peakValue)
		{
			peakValue = histogram[pixelValue];
			peak = pixelValue;
		}
	}

	unsigned char tail = 0; // least common pixel value, and if there are more pixel values with the same count, the farthest one away from the peak
	unsigned long tailValue = 999999999;
	if (peak < 128)
	{
		for (unsigned int pixelValue = peak; pixelValue < 256; pixelValue++)
		{
			if (histogram[pixelValue] <= tailValue)
			{
				tail = pixelValue;
				tailValue = histogram[pixelValue];
			}
		}
	}
	else
	{
		for (int pixelValue = peak; pixelValue >= 0; pixelValue--)
		{
			if (histogram[pixelValue] <= tailValue)
			{
				tail = pixelValue;
				tailValue = histogram[pixelValue];
			}
		}
	}

	// m = (y1 - y2) / (x1 - x2)

	double slope = (double)(histogram[peak] - histogram[tail]) / (double)(peak - tail);

	// y = m * x + k <=> k = y - m * x

	double yIntercept = (double)histogram[peak] - slope * (double)peak;

	double maxDistance = -99999.0;
	unsigned int optimalThresh = 0;

	if (peak > tail)
	{
		for (int pixelValue = tail; pixelValue < peak; pixelValue++)
		{
			double distance = abs(yIntercept + slope * pixelValue - histogram[pixelValue]) / sqrt(1 + pow(slope, 2));
			
			if (distance > maxDistance)
			{
				maxDistance = distance;
				optimalThresh = pixelValue;
			}
		}
	}
	else
	{
		for (int pixelValue = peak; pixelValue < tail; pixelValue++)
		{
			double distance = abs(yIntercept + slope * pixelValue - histogram[pixelValue]) / sqrt(1 + pow(slope, 2));
			if (distance > maxDistance)
			{
				maxDistance = distance;
				optimalThresh = pixelValue;
			}
		}
	}
	return threshold(grayscale, (unsigned char)optimalThresh);
}

// adaptive local thresholding
void getMask(Image input, unsigned int y, unsigned int x, unsigned int ksize, int* maskValues)
{
	if (input.isNull() || input.getType() != PixelType::GRAY)
	{
		return;
	}
	if (y < 0 || x < 0 || y >= input.getHeight() || x >= input.getWidth())
	{
		return;
	}
	//if (maskValues != nullptr) // ai grija sa initializezi la nullptr in functia principala
	//{
	//	delete[] maskValues;
	//	maskValues = new int[ksize * ksize];
	//}
	int kernelBorder = ksize / 2;
	int kernelUpBorder = y - kernelBorder;
	int kernelDownBorder = y + kernelBorder;
	int kernelLeftBorder = x - kernelBorder;
	int kernelRightBorder = x + kernelBorder;
	unsigned int kernelIndex = 0;

	for (int ky = kernelUpBorder; ky <= kernelDownBorder; ky++)
	{
		for (int kx = kernelLeftBorder; kx <= kernelRightBorder; kx++)
		{
			if (ky >= input.getHeight() || ky < 0 || kx >= input.getWidth() || kx < 0)
			{
				maskValues[kernelIndex] = -1;
			}
			else
			{
				maskValues[kernelIndex] = input[ky][kx].getValue(0);
			}
			kernelIndex++;
		}
	}
}
Image bernsenThresholding(Image input, unsigned int ksize)
{
	if (input.isNull() || ksize % 2 == 0 || ksize > 1000)
	{
		Image img;
		return img;
	}
	Image grayscale = convert2Gray(input);
	int* maskValues = new int[ksize * ksize];

	for (size_t y = 0; y < grayscale.getHeight(); y++)
	{
		for (size_t x = 0; x < grayscale.getWidth(); x++)
		{
			getMask(grayscale, y, x, ksize, maskValues);
			int maskMax = 0;
			int maskMin = 256;
			for (size_t kIndex = 0; kIndex < ksize * ksize; kIndex++)
			{
				if (maskValues[kIndex] != -1)
				{
					if (maskValues[kIndex] > maskMax)
						maskMax = maskValues[kIndex];
					if (maskValues[kIndex] < maskMin)
						maskMin = maskValues[kIndex];
				}
			}
			unsigned char thresh = (maskMax + maskMin) / 2;
			if (grayscale[y][x].getValue(0) > thresh)
				grayscale[y][x].setValue(255, 0);
			else
				grayscale[y][x].setValue(0, 0);
		}
	}
	delete[] maskValues;
	return grayscale;
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