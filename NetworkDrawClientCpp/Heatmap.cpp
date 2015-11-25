#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML\Graphics.hpp>
#include <ppl.h>
#include <iostream>
#include "Heatmap.h"
#include "App.h"
#include "Packet.h"
#include "PacketSender.h"

float Heatmap::Remap(float val, float min, float max, float targetMin, float targetMax)
{
	float retVal = 0.0f;

	float oldRange = (max - min);
		if (oldRange == 0)
		{
			retVal = targetMin;
		}

		else
		{
			float newRange = (targetMax - targetMin);
			retVal = (((val - min) * newRange) / oldRange) + targetMin;
		}

	return retVal;
}

sf::Color& Heatmap::GetColorFromIntensity(float intensity)
{
	sf::Color retVal = sf::Color();

	if (intensity > 0.8)
	{
		float colIntensity = Remap(intensity, 0.8f, 1.0f, 0.0f, 1.0f);

		retVal.r = colIntensity * 255;
		retVal.g = colIntensity * 255;
		retVal.b = colIntensity * 255;
	}

	else if (intensity > 0.64)
	{
		float colIntensity = Remap(intensity, 0.64f, 0.8f, 0.0f, 1.0f);

		retVal.r = colIntensity * 255;
		retVal.g = 0;
		retVal.b = 0;
	}

	else if (intensity > 0.48)
	{
		float colIntensity = Remap(intensity, 0.48f, 0.64f, 0.0f, 1.0f);

		retVal.r = colIntensity * 255;
		retVal.g = colIntensity * 255;
		retVal.b = 0;
	}

	else if (intensity > 0.32)
	{
		float colIntensity = Remap(intensity, 0.32f, 0.48f, 0.0f, 1.0f);

		retVal.r = 0;
		retVal.g = colIntensity * 255;
		retVal.b = 0;
	}

	else if (intensity > 0.16)
	{
		float colIntensity = Remap(intensity, 0.16f, 0.32f, 0.0f, 1.0f);

		retVal.r = 0;
		retVal.g = colIntensity * 255;
		retVal.b = colIntensity * 255;
	}

	else if (intensity > 0)
	{
		float colIntensity = Remap(intensity, 0.0f, 0.16f, 0.0f, 1.0f);

		retVal.r = 0;
		retVal.g = 0;
		retVal.b = colIntensity * 255;
	}

	return retVal;
}

int Heatmap::GetMaxValue()
{
	int maxValue = 0;

	for (size_t i = 0; i < mPosFrequency.size(); ++i)
	{
		if (mPosFrequency[i] > maxValue)
		{
			maxValue = mPosFrequency[i];
		}
	}

	return maxValue;
}

Heatmap::Heatmap()
{
	updatingWindowSize = true;

	App &appInst = App::Instance();

	mIntensityMultiplier = 1;

	mTexture = new sf::Texture();
	sf::Vector2u windowSize = appInst.GetWindow()->getSize();

	mTexture->create(windowSize.x, windowSize.y);

	mSprite = new sf::Sprite(*mTexture);

	mTexData = new sf::Image();
	mTexData->create(windowSize.x, windowSize.y);

	mDrawEnabled = false;

	for (size_t i = 0; i < windowSize.y; ++i)
	{
		for (size_t j = 0; j < windowSize.x; ++j)
		{
			mPosFrequency.push_back(0);
		}
	}

	updatingWindowSize = false;
}

Heatmap::~Heatmap()
{
}

void Heatmap::UpdateWindowSize(int width, int height)
{
	sf::Vector2u texDataSize = mTexData->getSize();

	//Both our new window dimensions are the same.
	if (texDataSize.x == width && texDataSize.y == height)
	{
		return;
	}

	//Set our flags so we are not trying to modify the texture in another thread.
	updatingWindowSize = true;

	//Delete our existing heatmap representations.
	DELETE_NULLIFY(mSprite);
	DELETE_NULLIFY(mTexture);
	DELETE_NULLIFY(mTexData);

	//Create our new heatmap representations with the correct size.
	mTexture = new sf::Texture();
	sf::Vector2u windowSize = App::Instance().GetWindow()->getSize();

	mTexture->create(width, height);

	mSprite = new sf::Sprite(*mTexture);

	mTexData = new sf::Image();
	mTexData->create(width, height);

	//Update our frequency array to contain the new values
	int arrayLength = width * height;
	int diff = arrayLength - mPosFrequency.size();

	if (diff > 0)
	{
		//Our new array will be larger. First zero our values, then add the new elements.
		for (size_t i = 0; i < mPosFrequency.size(); ++i)
		{
			mPosFrequency[i] = 0;
		}

		for (size_t i = 0; i < diff; ++i)
		{
			mPosFrequency.push_back(0);
		}
	}

	else
	{
		//Our new array is smaller. Pop off our values, then zero our values.
		for (size_t i = 0; i > diff; --i)
		{
			mPosFrequency.pop_back();
		}

		for (size_t i = 0; i < mPosFrequency.size(); ++i)
		{
			mPosFrequency[i] = 0;
		}
	}

	updatingWindowSize = false;
}

void Heatmap::DrawHeatmap(sf::RenderTarget& rt)
{
	if (mDrawEnabled && !updatingWindowSize)
	{
		UpdateTexture();
		rt.draw(*mSprite);
	}
}

void Heatmap::SaveToFile(const std::string& filename)
{
	if (!updatingWindowSize)
	{
		UpdateTexture();
		mTexData->saveToFile(filename);
	}
}

void Heatmap::SendToServer()
{
	if (!updatingWindowSize)
	{
		UpdateTexture();

		App &appInst = App::Instance();
		sf::Window* window = appInst.GetWindow();
		const sf::Vector2u windowSize = window->getSize();

		for(size_t i = 0; i< mPosFrequency.size(); ++i)
		{
			int posX = i % windowSize.x;
			int posY = i / windowSize.x;

			sf::Color outputColor = mTexData->getPixel(posX, posY);

			PacketPixel pixel;
			pixel.type = Packet::e_pixel;

			pixel.x = posX;
			pixel.y = posY;

			pixel.r = outputColor.r;
			pixel.g = outputColor.g;
			pixel.b = outputColor.b;

			//Send this Packet
			//App::Instance().GetPacketSender()->AddPacketData((char*)&pixel, sizeof(pixel));
			App::Instance().GetPacketSender()->SendImmediate((char*)&pixel, sizeof(pixel));
		}
	}
}

void Heatmap::UpdateTexture()
{
	App &appInst = App::Instance();
	sf::Window* window = appInst.GetWindow();
	const sf::Vector2u windowSize = window->getSize();

	int maxValue = GetMaxValue();

	concurrency::parallel_for((size_t)0, mPosFrequency.size(), [&](size_t i)
	{
		int posX = i % windowSize.x;
		int posY = i / windowSize.x;

		sf::Color outputColor = GetColorFromIntensity(std::exp(mPosFrequency[i] / (float)maxValue * mIntensityMultiplier));

		mTexData->setPixel(posX, posY, outputColor);
	});

	mTexture->update(*mTexData);
}