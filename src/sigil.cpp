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
	// TODO: make opposite function to reverse dir
	// HexPoint prevHex = grid.hexNeighbor(hex, opposite(dir));
	int maxTries = 10;
	while (maxTries > 0 && (!grid.isEdge(hex, dir) || !grid.isOccupied(nextHex))) {
		hex = grid.hexNeighbor(hex, dir);
		nextHex = grid.hexNeighbor(hex, dir);
		maxTries--;
	}
	
	position = grid.hexPosition(hex);

	// int sigilIdx = 0;
	// if (grid.isOccupied(prevHex)) {
	// 	// TODO: add getSigilIcon on Grid
	// 	SigilIcon prevIcon = grid.getSigilIcon(prevHex);
	// 	sigilIdx = prevIcon.index;
	// }

	// return sigilIdx;
}

void Sigil::resize(int width, int height) {
	
}
