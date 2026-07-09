#include "sigil.hpp"

#include <raylib.h>

void Sigil::load() {
	
}

void Sigil::render() const {
	DrawPoly(position, 6, sigilSize, 0.0f, PURPLE);
	DrawPolyLines(position, 6, sigilSize, 0.0f, BLACK);
	const char* sigilValue = TextFormat("%d", value);
	DrawText(sigilValue, position.x, position.y, 30, BLACK);
}

void Sigil::update(const Grid& grid, Direction dir) {
	HexPoint nextHex = grid.hexNeighbor(hex, dir);
	if (nextHex != hex) {
		hex = nextHex;
		position = grid.hexPosition(hex);
	}
}

void Sigil::resize(int width, int height) {
	
}
