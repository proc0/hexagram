#include "grid.hpp"
#include "types.hpp"

#include <raylib.h>

void Grid::load() {
	generate(gridSize);

	// map.at(activeHex).isActive = true;
}

// k = number of hex layers
void Grid::generate(int k) {
	for (int q = -k; q <= k; q++) {
	    int r1 = fmax(-k, -q - k);
	    int r2 = fmin( k, -q + k);
	    // bool isRowEdge = q == -k || q == k;
	    for (int r = r1; r <= r2; r++) {
	    	HexPoint hex = { q, r };
	    	// bool isColEdge = r == r1 || r == r2;
			// map[hex] = { hex, project(hex) };
			map.insert({ hex, { hex, project(hex) } });
	    }
	}
}

void Grid::render() const {
	for (auto& [point, hex] : map ) {
		renderHex(hex);
	}
}

void Grid::renderHex(const HexState& hex) const {
    DrawPoly(hex.position, 6, hexSize, 0.0f, BEIGE);
    DrawPolyLines(hex.position, 6, hexSize, 0.0f, BLACK);
    const char* pointLabel = TextFormat("(%d, %d, %d)", hex.point.q, hex.point.r, hex.point.s);
    DrawText(pointLabel, hex.position.x-30.0f, hex.position.y-9.0f, 18, BLACK);
}

void Grid::update() {
	// if (IsKeyPressed(KEY_W)) {
	// 	updateHex(Direction::UP);
	// } else if (IsKeyPressed(KEY_E)) {
	// 	updateHex(Direction::UP_RIGHT);
	// } else if (IsKeyPressed(KEY_D)) {
	// 	updateHex(Direction::DOWN_RIGHT);
	// } else if (IsKeyPressed(KEY_S)) {
	// 	updateHex(Direction::DOWN);
	// } else if (IsKeyPressed(KEY_A)) {
	// 	updateHex(Direction::DOWN_LEFT);
	// } else if (IsKeyPressed(KEY_Q)) {
	// 	updateHex(Direction::UP_LEFT);
	// }
}

void Grid::updateHex(Direction dir) {
	// map.at(activeHex).isActive = false;
	// HexPoint nextHex = hexNeighbor(activeHex, dir);
	// activeHex = nextHex;
	// map.at(activeHex).isActive = true;
}

void Grid::placeIcon(HexPoint hex, SigilIcon icon) {
	if(!isValid(hex)) return;

	map.at(hex).icon = icon;
	map.at(hex).isOccupied = true;
}

void Grid::removeIcon(HexPoint hex) {
	if(!isValid(hex)) return;

	map.at(hex).isOccupied = false;
}

HexPoint Grid::inject(Vector2 point) {
    HexPoint hex = HexPoint(
    	(point.x - origin.x) / hexSize, 
        (point.y - origin.y) / hexSize);
    float q = view.b0 * hex.q + view.b1 * hex.r;
    float r = view.b2 * hex.q + view.b3 * hex.r;

	return { static_cast<int>(roundf(q)), static_cast<int>(roundf(r)) };
}

Vector2 Grid::project(HexPoint hexPoint) {
    float x = (view.f0 * hexPoint.q + view.f1 * hexPoint.r) * hexSize;
    float y = (view.f2 * hexPoint.q + view.f3 * hexPoint.r) * hexSize;
	
	return { x + origin.x, y + origin.y };
}

void Grid::resize(int width, int height) {

}

HexPoint Grid::hexAdd(HexPoint a, HexPoint b) const {
    return HexPoint(a.q + b.q, a.r + b.r, a.s + b.s);
}

HexPoint Grid::hexSubtract(HexPoint a, HexPoint b) const {
    return HexPoint(a.q - b.q, a.r - b.r, a.s - b.s);
}

HexPoint Grid::hexMultiply(HexPoint a, int k) const {
    return HexPoint(a.q * k, a.r * k, a.s * k);
}

int Grid::hexLength(HexPoint a) const {
    return int((abs(a.q) + abs(a.r) + abs(a.s)) / 2);
}

int Grid::hexDistance(HexPoint a, HexPoint b) const {
    return hexLength(hexSubtract(a, b));
}

HexPoint Grid::hexNeighbor(HexPoint hex, Direction dir) const {
    HexPoint neighbor = hexAdd(hex, directions[dir]);
    if (abs(neighbor.q) > gridSize || abs(neighbor.r) > gridSize || abs(neighbor.s) > gridSize) {
    	return hex;
    }
    return neighbor;
}

Vector2 Grid::hexPosition(int q, int r, int s) const {
    if (abs(q) > gridSize || abs(r) > gridSize || abs(s) > gridSize) {
    	return Vector2({ 0.0f, 0.0f });
    }

    return map.at(HexPoint(q, r, s)).position;
}

Vector2 Grid::hexPosition(HexPoint p) const {
    if (abs(p.q) > gridSize || abs(p.r) > gridSize || abs(p.s) > gridSize) {
    	return Vector2({ 0.0f, 0.0f });
    }

    return map.at(p).position;
}

bool Grid::isEdge(HexPoint hex, Direction dir) const {
	bool result = false;

	switch (dir) {
		case Direction::UP:
			result = hex.s == 3 || hex.r == -3;
			break;
		case Direction::UP_RIGHT:
			result = hex.r == -3 || hex.q == 3;
			break;
		case Direction::DOWN_RIGHT:
			result = hex.q == 3 || hex.s == -3;
			break;
		case Direction::DOWN:
			result = hex.s == -3 || hex.r == 3;
			break;
		case Direction::DOWN_LEFT:
			result = hex.r == 3 || hex.q == -3;
			break;
		case Direction::UP_LEFT:
			result = hex.q == -3 || hex.s == 3;
			break;
		default:
			result = false;
	}

	return result;
}

bool Grid::isOccupied(HexPoint hex) const {
	if (!isValid(hex)) return false;

	return map.at(hex).isOccupied;
}

bool Grid::isValid(HexPoint p) const {
	if (abs(p.q) > gridSize || abs(p.r) > gridSize || abs(p.s) > gridSize) {
    	return false;
    }

    return true;
}