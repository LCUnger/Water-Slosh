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
	Grid2D(int width, int height) : width_(width), height_(height) {}
	Grid2D(int width, int height, GridLocation location) : width_(width), height_(height), location_(location) {}


  int get_width() const { return width_; }
	int get_height() const { return height_; }


private:
	int width_{};
	int height_{};
	GridLocation location_{ GridLocation::CellCenter };
};