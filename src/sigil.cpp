#include "sigil.hpp"
#include "types.hpp"

#include <raylib.h>
#include <utility>

void Sigil::load() {
	
}
// TODO: memoize this in a member var
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
	int mergeIndex = 0;
	int chainIndex = 0;
	
	TraceLog(LOG_INFO, "----- Sigil %d (%d) Update -----", effigy.index, effigy.value);
	// save starting position
	HexPoint sourceHex = hex;
	// initial target hex in the moving direction
	HexPoint targetHex = grid.hexNeighbor(hex, dir);
	TraceLog(LOG_INFO, "Target Neighbor Hex: %d %d %d", targetHex.q, targetHex.r, targetHex.s);

	// if neigbhor returns source hex, sigil cannot move
	if (targetHex == sourceHex) {
		TraceLog(LOG_INFO, "CANNOT MOVE");
		return std::make_pair(mergeIndex, chainIndex);
	}

	// safety, max moves < number of hexes along direction
	// depends on hex grid size, which may vary 
	// TODO: either remove or use something like grid.getSideLength
	int maxMoves = 30;
	while (maxMoves > 0 && !grid.isEdge(hex, dir) && !grid.isOccupied(targetHex)) {
		// move sigil
		hex = targetHex;
		// set next target neighbor
		targetHex = grid.hexNeighbor(hex, dir);
		maxMoves--;
	}

	// check if next hex after destination has a sigil
	if (grid.isOccupied(targetHex)) {
		Effigy targetEffigy = grid.getEffigy(targetHex);
		// if the values are the same send the mergeIndex index to World
		// TODO: separate merge logic into its own method
		if (targetEffigy.value == effigy.value) {			
			TraceLog(LOG_INFO, "Merging values: %d %d", targetEffigy.value, effigy.value);
			// update hex to new target
			hex = targetHex;
			// return target index for merging
			mergeIndex = targetEffigy.index;
		}
		// if sigil has not moved
		if (sourceHex == hex) {
			// target hex is occupied and sigil cannot move
			// sigil can still merge but moves end here
			TraceLog(LOG_INFO, "Cannot move forward or merge");
			return std::make_pair(mergeIndex, chainIndex);
		}
	}

	TraceLog(LOG_INFO, "Destination Hex: %d %d %d", hex.q, hex.r, hex.s);
	// update screen position
	// position = grid.hexPosition(hex);

	// get the hex behind in the opposite direction (will return sourceHex if edge)
	HexPoint chainHex = grid.hexNeighbor(sourceHex, oppositeDir(dir));
	// if the hex behind the current one (before moving)
	// contains a sigil, return the index to move it
	if (chainHex != sourceHex && grid.isOccupied(chainHex)) {
		// retrieve effigy to get sigil index to return to World
		Effigy chainEffigy = grid.getEffigy(chainHex);
		chainIndex = chainEffigy.index;
		TraceLog(LOG_INFO, "Chain Hex %d (%d) at %d %d %d", chainEffigy.index, chainEffigy.value, chainHex.q, chainHex.r, chainHex.s);
	}

	// return merge sigil index to merge with this one
	// and a chain sigil to call subsequent sigil update
	return std::make_pair(mergeIndex, chainIndex);
}

Vector2 Sigil::getPosition() const {
	return position;
}

void Sigil::setPosition(Vector2 pos) {
	position = pos;
}

HexPoint Sigil::getHex() const {
	return hex;
}

void Sigil::setHex(HexPoint point) {
	hex = point;
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

void Sigil::log(const char* context) const {
	TraceLog(LOG_INFO, "Sigil %d (%d) at (%d,%d,%d): %s", effigy.index, effigy.value, hex.q, hex.r, hex.s, context);
}