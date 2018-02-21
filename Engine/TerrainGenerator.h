#pragma once
#include "PerlinNoise.h"
#include <limits>
#include <array>

class TerrainGenerator
{
public:
	TerrainGenerator(int mapWidth, int mapHeight, int seed, float noiseScale,int octaves, float persistance, float lacunarity)
		:
		mapWidth(mapWidth),
		mapHeight(mapHeight),
		seed(seed),
		noiseScale(noiseScale),
		octaves(octaves),
		persistance(persistance),
		lacunarity(lacunarity)
	{
	}
	std::vector<float> GenerateMap() 
	{
		return GenerateNoiseMap(mapWidth, mapHeight, seed, noiseScale, octaves, persistance, lacunarity);
	}
	void Loop()
	{
		for (int i = 0; i < mapWidth * mapHeight; i++)
		{
			float this_ = noiseMap[i];
		}
	}
private:
	int mapWidth;
	int mapHeight;
	float noiseScale;

	int seed;
	int octaves;
	float persistance;
	float lacunarity;
	std::vector<float> noiseMap;
private:
	std::vector<float> GenerateNoiseMap(int mapWidth, int mapHeight, int seed, float scale, int octaves, float persistance, float lacunarity)
	{
		noiseMap.resize(mapWidth*mapHeight);

		if (scale <= 0)
		{
			scale = 0.00001f;
		}
		float maxNoiseHeight = -std::numeric_limits<float>::max();
		float minNoiseHeight = std::numeric_limits<float>::max();

		siv::PerlinNoise pNoise = siv::PerlinNoise(seed);
		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {

				float amplitude = 1;
				float frequency = 1;
				float noiseHeight = 0;

				for (int i = 0; i < octaves; i++) {
					float sampleX = x / scale * frequency;
					float sampleY = y / scale * frequency;


					float perlinValue = (float)pNoise.noise(sampleX, sampleY);
					noiseHeight += perlinValue * amplitude;

					amplitude *= persistance;
					frequency *= lacunarity;
				}
				if (noiseHeight > maxNoiseHeight) maxNoiseHeight = noiseHeight;
				else if (noiseHeight < minNoiseHeight) minNoiseHeight = noiseHeight;
				noiseMap[mapWidth * y + x] = noiseHeight;
			}
		}

		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {
				noiseMap[mapWidth * y + x] = (noiseMap[mapWidth * y + x] - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
			}
		}
		return noiseMap;
	}
};