#pragma once
#include <vector>

#include "TerrainGenerator.h"
#include "Graphics.h"
#include "Rect.h"
#include "Vec2.h"
#include "Keyboard.h"
#include "Layer.h"

class World
{
private:
	enum class TerrainType
	{
		water,
		sand,
		grass,
		rock,
		highRock
	};
public:
	World(Graphics& gfx);
	void SetTerrain(int seed, int mapWidth, int mapHeight);
	void Update(Keyboard& kbd);
	void SetLayer(const TerrainType& type);
	void DrawTerrain();
	void DrawCell(int x, int y, int i, int j);
private:
	Graphics& gfx;
	RectI worldRect;		// in tiles
	RectI worldRectPos;		// in amount of Rects
	Vei2 TopLeftDraw;
	Layer visible = {};

	TerrainType curLayerType;
	RectI curLocalRect;
	Vei2 curLocalPos;
	
	std::vector<TerrainType> TerrainTypeMap;
	std::vector<RectI> LocalMaps;
private:
	// dimensions world map
	int widthWorld;									// in tile units
	int heightWorld;								// in tile units  
	// dimensions of vector LocalMaps
	int widthVectorOfRects;							// in tiles
	int heightVectorOfRects;						// in tiles
	// dimensions DrawTiles
	static constexpr int tileWidth = 40;			// in pixel units
	static constexpr int tileHeight = 40;			// in pixel units
	// dimensions Amount visible tiles
	static constexpr int localMapWidth = 5;			// in tile units
	static constexpr int localMapHeight = 5;		// in tile units


};