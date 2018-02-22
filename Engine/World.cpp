#include "World.h"
#include <cassert>

World::World(Graphics & gfx)
	:
	gfx(gfx)
{
}

void World::SetTerrain(int seed, int mapWidth, int mapHeight)
{
	widthWorld = mapWidth;
	heightWorld = mapHeight;
	widthVectorOfRects = mapWidth / localMapWidth;
	heightVectorOfRects = mapHeight / localMapHeight;

	worldRectPos = { Vei2(0,0),widthVectorOfRects, heightVectorOfRects };
	worldRect = { Vei2(0,0),widthWorld,heightWorld };
	TopLeftDraw = { Graphics::ScreenWidth / 2 - ((localMapWidth * tileWidth) / 2),Graphics::ScreenHeight / 2 - ((localMapHeight * tileHeight) / 2) };

	TerrainGenerator tg = TerrainGenerator(mapWidth, mapHeight, seed, 20, 4, 0.5, 2);
	std::vector<float> noiseMap = tg.GenerateMap();
	curLayerType = TerrainType::grass;

	TerrainTypeMap.resize(mapWidth * mapHeight, TerrainType::rock);
	LocalMaps.resize(widthVectorOfRects * heightVectorOfRects);
	
	// init local maps
	for (int y = 0; y < widthVectorOfRects; y++) {
		for (int x = 0; x < heightVectorOfRects; x++) {
			LocalMaps[widthVectorOfRects * y + x] = RectI(Vei2(x * localMapWidth, y * localMapHeight), localMapWidth, localMapHeight);
		}
	}

	// assign types
	for (int y = 0; y < heightWorld; y++) {
		for (int x = 0; x < widthWorld; x++) {
			if (noiseMap[widthWorld * y + x] <= 0.3f)
			{
				TerrainTypeMap[widthWorld * y + x] = TerrainType::water;
			}
			else if (noiseMap[widthWorld * y + x] <= 0.4f)
			{
				TerrainTypeMap[widthWorld * y + x] = TerrainType::sand;
			}
			else if (noiseMap[widthWorld * y + x] <= 0.7f)
			{
				TerrainTypeMap[widthWorld * y + x] = TerrainType::grass;
			}
			else if (noiseMap[widthWorld * y + x] <= 0.9f)
			{
				TerrainTypeMap[widthWorld * y + x] = TerrainType::rock;
			}
			else if (noiseMap[widthWorld * y + x] <= 1.0f)
			{
				TerrainTypeMap[widthWorld * y + x] = TerrainType::highRock;
			}
		}
	}

	curLocalPos = { widthVectorOfRects / 2, heightVectorOfRects / 2 };
	curLocalRect = LocalMaps[widthVectorOfRects * curLocalPos.y + curLocalPos.x];
	SetLayer(TerrainType::grass);
	visible.CalculateWalls();
}

void World::Update(Keyboard & kbd)
{
	while (!kbd.KeyIsEmpty())
	{
		newWalls = false;
		const Keyboard::Event e = kbd.ReadKey();
		if (e.IsRelease())
		{
			if (e.GetCode() == 'J' && worldRectPos.Contains(curLocalPos + Vei2(-1, 0)))
			{
				curLocalPos += Vei2(-1, 0);
				newWalls = true;
			}
			if (e.GetCode() == 'K' && worldRectPos.Contains(curLocalPos + Vei2(0, 1)))
			{
				curLocalPos += Vei2(0, 1);
				newWalls = true;
			}
			if (e.GetCode() == 'L' && worldRectPos.Contains(curLocalPos + Vei2(1, 0)))
			{
				curLocalPos += Vei2(1, 0);
				newWalls = true;
			}
			if (e.GetCode() == 'I' && worldRectPos.Contains(curLocalPos + Vei2(0, -1)))
			{
				curLocalPos += Vei2(0, -1);
				newWalls = true;
			}
			if (e.GetCode() == '1')
			{
				curLayerType = TerrainType::water;
				newWalls = true;
			}
			if (e.GetCode() == '2')
			{
				curLayerType = TerrainType::sand;
				newWalls = true;
			}
			if (e.GetCode() == '3')
			{
				curLayerType = TerrainType::grass;
				newWalls = true;
			}
			if (e.GetCode() == '4')
			{
				curLayerType = TerrainType::rock;
				newWalls = true;
			}
		}
		curLocalRect = LocalMaps[widthVectorOfRects * curLocalPos.y + curLocalPos.x];
		if (newWalls)
		{
			SetLayer(curLayerType);
			visible.CalculateWalls();
			newWalls = false;
		}
	}
}

void World::SetLayer(const TerrainType & type)
{
	visible.ClearRects();
	visible.ClearWalls();
	int yTemp = curLocalRect.top;
	int xTemp = curLocalRect.left;
	int xEnd = curLocalRect.left + curLocalRect.GetWidth();
	int yEnd = curLocalRect.top + curLocalRect.GetHeight();
	Vei2 TopLeftPos = TopLeftDraw;

	if (curLocalRect.left > 0) 
	{ 
		xTemp--;
		TopLeftPos.x -= tileWidth;
	}
	if (curLocalRect.top > 0) 
	{
		yTemp--;
		TopLeftPos.y -= tileWidth;
	}

	if (xEnd < widthWorld) 
	{
		xEnd++;
	}
	if (yEnd < heightWorld) 
	{
		yEnd++;
	}

	for (int j = 0, y = yTemp; y < yEnd; y++, j++) {
		for (int i = 0, x = xTemp; x < xEnd; x++, i++) {
			auto t = TerrainTypeMap[widthWorld * y + x];
			if (t == type)
			{
				visible.AddRect(RectI(Vei2(TopLeftPos.x + (i*tileWidth), TopLeftPos.y + (j * tileHeight)), tileWidth, tileHeight));
			}
		}
	}
}

void World::DrawTerrain()
{
	visible.DrawWalls(gfx, RectI(Vei2(TopLeftDraw.x, TopLeftDraw.y), tileWidth * localMapWidth, tileHeight * localMapHeight));
}

void World::DrawTerrainMap()
{
	for (int y = curLocalRect.top, j = 0; y < curLocalRect.top + curLocalRect.GetHeight(); y++, j++) {
		for (int x = curLocalRect.left, i = 0; x < curLocalRect.left + curLocalRect.GetWidth(); x++, i++) {
			DrawCell(x, y, i, j);
		}
	}
}

std::vector<LineSegment> World::GetWalls()
{
	return visible.GetWalls();
}

void World::DrawCell(int x, int y, int i, int j)
{
	switch (TerrainTypeMap[widthWorld * y + x])
		{
		case TerrainType::water:
		{
			gfx.DrawRectDim(TopLeftDraw.x + (i*tileWidth), TopLeftDraw.y + (j * tileHeight), tileWidth, tileHeight, Color{ 15,79,158 });
			break;
		}
		case TerrainType::sand:
		{
			gfx.DrawRectDim(TopLeftDraw.x + (i*tileWidth), TopLeftDraw.y + (j * tileHeight), tileWidth, tileHeight, Color{ 158,148,15 });
			break;
		}
		case TerrainType::grass:
		{
			gfx.DrawRectDim(TopLeftDraw.x + (i*tileWidth), TopLeftDraw.y + (j * tileHeight), tileWidth, tileHeight, Color{ 26,112,15 });
			break;
		}
		case TerrainType::rock:
		{
			gfx.DrawRectDim(TopLeftDraw.x + (i*tileWidth), TopLeftDraw.y + (j * tileHeight), tileWidth, tileHeight, Colors::Gray);
			break;
		}
		case TerrainType::highRock:
		{
			gfx.DrawRectDim(TopLeftDraw.x + (i*tileWidth), TopLeftDraw.y + (j * tileHeight), tileWidth, tileHeight, Colors::LightGray);
			break;
		}
	}
}
