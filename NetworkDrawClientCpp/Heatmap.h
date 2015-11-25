#ifndef HEATMAP_H
#define HEATMAP_H
#include <vector>

#include "Macros.h"

namespace sf
{
	class Color;
	class Sprite;
	class RenderTarget;
	class Texture;
	class Image;
}

class Heatmap
{
public:
	Heatmap();
	~Heatmap();

	void DrawHeatmap(sf::RenderTarget& rt);
	void SaveToFile(const std::string& filename);
	void SendToServer();

	void UpdateTexture();
	float Remap(float val, float min, float max, float targetMin, float targetMax);
	sf::Color& GetColorFromIntensity(float intensity);
	int GetMaxValue();
	void UpdateWindowSize(int width, int height);

	std::vector<int> mPosFrequency;
	PUBLIC_DATA_PROPERTY(bool, DrawEnabled);
private:
	sf::Sprite* mSprite;
	sf::Texture* mTexture;
	sf::Image* mTexData;

	bool updatingWindowSize;
	float mIntensityMultiplier;
};

#endif//HEATMAP_H