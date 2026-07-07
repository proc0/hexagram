#include "grid.hpp"

#include <raylib.h>
#include <raymath.h>

// NOTE: in order to make this more flexible use <numbers> and std::numbers::sqrt2
// consteval float sq3over2() { return std::sqrtf(3.0f) * 0.5f }
constexpr float SQRT3_OVER_2 = 0.86602540378f; 
constexpr float RAD_60 = 60.0f*DEG2RAD;

void Grid::load() {
	float originX = window.halfWidthf;
	float originY = window.halfHeightf;
	Vector2 origin = Vector2({ originX, originY });

	// Vector2 hexCenter = Vector2({ 0.0f, 0.0f });
	// drawHex(Vector2Add(hexCenter, origin));

	hexes.reserve(20);
	hexes.emplace_back(Vector2({ 0.0f, 0.0f }), origin, false);
	// drawHex(Vector2Add(hexPos, origin));
	for (int i=0; i<6; ++i) {		
		Vector2 hexPos = Vector2({ 0.0f, radius*-SQRT3_OVER_2*2.0f });
		hexPos = Vector2Rotate(hexPos, RAD_60*i);
		Vector2 hexProj = Vector2Add(hexPos, origin);
		hexes.emplace_back(hexPos, hexProj, false);

		Vector2 hexPos2 = hexPos*2.0f;
		Vector2 hexProj2 = Vector2Add(hexPos2, origin);
		hexes.emplace_back(hexPos2, hexProj2, false);

		Vector2 hexPos3 = Vector2Rotate(hexPos, RAD_60);
		Vector2 hexProj3 = Vector2Add(Vector2Add(hexPos3, hexPos), origin);
		hexes.emplace_back(hexPos3, hexProj3, false);
	}
}

void Grid::renderGrid() const {

	for (const Hex& hex : hexes) {
		drawHex(hex.projection, hex.isClicked);
	}
	// float originX = window.halfWidthf;
	// float originY = window.halfHeightf;
	// Vector2 origin = Vector2({ originX, originY });


	// Vector2 hexCenter = Vector2({ 0.0f, 0.0f });
	// drawHex(Vector2Add(hexCenter, origin));

	// // drawHex(Vector2Add(hexPos, origin));
	// for (int i=0; i<6; ++i) {		
	// 	Vector2 hexPos = Vector2({ 0.0f, radius*-SQRT3_OVER_2*2.0f });
	// 	hexPos = Vector2Rotate(hexPos, RAD_60*i);
	// 	drawHex(Vector2Add(hexPos, origin));

	// 	Vector2 hexPos2 = hexPos*2.0f;
	// 	drawHex(Vector2Add(hexPos2, origin));

	// 	Vector2 hexPos3 = Vector2Rotate(hexPos, RAD_60);
	// 	drawHex(Vector2Add(Vector2Add(hexPos3, hexPos), origin));
	// }

}

void Grid::updateGrid() {
	for (Hex& hex : hexes) {
		if (CheckCollisionPointCircle(GetMousePosition(), hex.projection, radius)) {
			hex.isClicked = true;
		} else {
			hex.isClicked = false;
		}
	}
}

void Grid::drawHex(Vector2 position, bool isClicked) const {
    DrawPoly(position, 6, radius, 0.0f, isClicked ? YELLOW : BEIGE);
    DrawPolyLines(position, 6, radius, 0.0f, BLACK);
}

void Grid::resize(int width, int height) {
	
}
