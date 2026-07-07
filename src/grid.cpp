#include "grid.hpp"

#include <raylib.h>
#include <raymath.h>

// NOTE: in order to make this more flexible use <numbers> and std::numbers::sqrt2
// consteval float sq3over2() { return std::sqrtf(3.0f) * 0.5f }
constexpr float SQRT3_OVER_2 = 0.86602540378f; 
constexpr float RAD_60 = 60.0f*DEG2RAD;

void Grid::renderGrid() const {
	float originX = window.halfWidthf;
	float originY = window.halfHeightf;
	Vector2 origin = Vector2({ originX, originY });
	Vector2 hexPos = Vector2({ 0.0f, 0.0f });
	drawHex(Vector2Add(hexPos, origin));
	Vector2 hexPos2 = Vector2({ 0.0f, radius*-SQRT3_OVER_2*2.0f });
	drawHex(Vector2Add(hexPos2, origin));
	Vector2 hexPos3 = Vector2Rotate(hexPos2, RAD_60);
	drawHex(Vector2Add(hexPos3, origin));
	// drawHex(Vector2Add(findThirdPoint(hexPos, hexPos2), origin));
}

void Grid::updateGrid() {
	
}

void Grid::drawHex(Vector2 position) const {
    DrawPoly(position, 6, radius, 0.0f, BEIGE);
    DrawPolyLines(position, 6, radius, 0.0f, BLACK);
}

void Grid::resize(int width, int height) {
	
}
