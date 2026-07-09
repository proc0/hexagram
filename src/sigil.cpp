#include "sigil.hpp"

#include <raylib.h>

void Sigil::load() {
	
}

void Sigil::render() const {
	DrawPoly(position, 6, sigilSize, 0.0f, PURPLE);
	DrawPolyLines(position, 6, sigilSize, 0.0f, BLACK);
	const char* sigilValue = TextFormat("%d", effigy.value);
	DrawText(sigilValue, position.x, position.y, 30, BLACK);
}

void Sigil::update(const Grid& grid, Direction dir) {
	HexPoint nextHex = grid.hexNeighbor(hex, dir);
	// TODO: make opposite function to reverse dir
	// HexPoint prevHex = grid.hexNeighbor(hex, oppositeDir(dir));
	int maxTries = 10;
	while (maxTries > 0 && (!grid.isEdge(hex, dir) || !grid.isOccupied(nextHex))) {
		// TODO: remove sigil from prevHex
		// grid.removeIcon(hex);
		hex = grid.hexNeighbor(hex, dir);
		// TODO: add sigil to current hex
		// grid.placeIdon(sigilIcon);
		nextHex = grid.hexNeighbor(hex, dir);
		maxTries--;
	}
	// TODO: add check for nextHex's sigil and check if it's the same
	// if (grid.isOccupied(nextHex) && grid.isSigilIcon(sigilIcon)) {
	//   // occupy nextHex, and update sigilIcon on grid
	//   hex = nextHex;
	//   grid.placeIcon(hex, sigilIcon);
	// }
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
