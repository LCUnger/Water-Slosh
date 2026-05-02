#pragma once

enum class GridLocation
{
	CellCenter,
	CellCorner,
	VerticalFace,
	HorizontalFace,
};



class Grid2D
{
public:
	Grid2D(int width, int height) : width(width), height(height) {}
	Grid2D(int width, int height, GridLocation location) : width(width), height(height), location(location) {}


	int getWidth() const { return width; }
	int getHeight() const { return height; }


private:
	int width;
	int height;
	GridLocation location{ GridLocation::CellCenter };
};