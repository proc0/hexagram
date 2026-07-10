#include "sigil.hpp"
#include "types.hpp"

#include <raylib.h>
#include <utility>

void Sigil::load() {
	
}

Color Sigil::getColor() const {
	Color col = GRAY;
	switch(effigy.value) {
		case 4:
			col = PINK;
			break;
		case 8:
			col = GREEN;
			break;
		case 16:
			col = YELLOW;
			break;
		case 32:
			col = ORANGE;
			break;
		case 64:
			col = RED;
			break;
		case 128:
			col = PURPLE;
			break;
		default:
			col = GRAY;
	}

	return col;
}

void Sigil::render() const {
	DrawPoly(position, 6, sigilSize, 0.0f, getColor());
	DrawPolyLines(position, 6, sigilSize, 0.0f, BLACK);
	const char* sigilValue = TextFormat("%d", effigy.value);
	float fontWidth = MeasureText(sigilValue, 42);
	DrawText(sigilValue, position.x-fontWidth*0.5f, position.y-21.0f, 42, WHITE);
}

std::pair<int, int> Sigil::update(const Grid& grid, Direction dir) {
	int mergeSigil = 0;
	int chainSigil = 0;
	
	TraceLog(LOG_INFO, "----- SIGIL %d (%d) UPDATE -----", effigy.index, effigy.value);
	// save initial starting position
	HexPoint currHex = hex;
	HexPoint nextHex = grid.hexNeighbor(hex, dir);
	TraceLog(LOG_INFO, "Next Hex: %d %d %d", nextHex.q, nextHex.r, nextHex.s);

	// if neigbhor returns current sigil hex or
	// the next hex is occupied, sigil cannot move
	if (nextHex == currHex) {
		TraceLog(LOG_INFO, "Cannot move forward");
		return std::make_pair(mergeSigil, chainSigil);
	}

	int maxTries = 10;
	while (maxTries > 0 && !grid.isEdge(hex, dir) && !grid.isOccupied(nextHex)) {
		// TraceLog(LOG_INFO, "Move forward");
		hex = nextHex;
		// if (grid.isEdge(hex, dir)) break;
		nextHex = grid.hexNeighbor(hex, dir);
		// if (grid.isOccupied(nextHex)) break;
		// TraceLog(LOG_INFO, "Next Hex: %d %d %d", nextHex.q, nextHex.r, nextHex.s);
		maxTries--;
	}

	// check if next hex after destination has a sigil
	if (grid.isOccupied(nextHex)) {
		Effigy nextEff = grid.getEffigy(nextHex);
		// if the values are the same send the mergeSigil index to World
		if (nextEff.value == effigy.value) {			
			TraceLog(LOG_INFO, "Merging values: %d %d", nextEff.value, effigy.value);
			// occupy nextHex, and update sigilIcon on grid
			hex = nextHex;
			mergeSigil = nextEff.index;
		}
	}

	// move the sigil to the resulting hex
	// grid.vacate(currHex);
	position = grid.hexPosition(hex);
	// grid.occupy(hex, effigy);
	TraceLog(LOG_INFO, "Destination Hex: %d %d %d", hex.q, hex.r, hex.s);

	// get the hex behind in the opposite direction (will return currHex if edge)
	HexPoint prevHex = grid.hexNeighbor(currHex, oppositeDir(dir));
	// if the hex behind the current one (before moving)
	// contains a sigil, return the index to move it
	if (prevHex != currHex && grid.isOccupied(prevHex)) {
		TraceLog(LOG_INFO, "Prev Hex is Occupied: %d %d %d", prevHex.q, prevHex.r, prevHex.s);
		// retrive effigy to get sigil index to return to World
		Effigy prevEff = grid.getEffigy(prevHex);
		TraceLog(LOG_INFO, "Prev Hex: %d (%d)", prevEff.index, prevEff.value);
		chainSigil = prevEff.index;
	}

	// return merge sigil index to merge with this one
	// and a chain sigil to call subsequent sigil update
	return std::make_pair(mergeSigil, chainSigil);
}

HexPoint Sigil::getHex() const {
	return hex;
}

Effigy Sigil::getEffigy() const {
	return effigy;
}

void Sigil::setEffigy(Effigy eff) {
	effigy = eff;
}

bool Sigil::isActive() const {
	return active;
}

void Sigil::enable() {
	active = true;
}

void Sigil::disable() {
	active = false;
}

void Sigil::resize(int width, int height) {
	
}
