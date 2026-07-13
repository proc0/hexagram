#include "grid.hpp"
#include "types.hpp"
#include "macros.hpp"

#include <raylib.h>

void Grid::load() {
	generate(gridSize);
}

void Grid::reset() {
  bgColor = BEIGE;
  borderColor = BLACK;
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
    DrawPoly(hex.position, 6, hexSize, 0.0f, hex.isOccupied ? YELLOW : bgColor);
    DrawPolyLines(hex.position, 6, hexSize, 0.0f, borderColor);
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


void Grid::phaseChange(int phase) {
    switch(phase) {
    	case 2:
            bgColor = BEIGE;
            borderColor = BLACK;
        case 32:
            bgColor = BEIGE;
            borderColor = BLACK;
            break;
        case 128:
            bgColor = GRAY;
            borderColor = BLACK;
            break;
        case 512:
            bgColor = DARKGRAY;
            borderColor = RAYWHITE;
            break;
        case 1024:
            bgColor = BLACK;
            borderColor = WHITE;
            break;
        default:
            return;
    }
}

void Grid::occupy(HexPoint hex, Effigy eff) {
	if(!isValid(hex)) return;

	map.at(hex).effigy = eff;
	map.at(hex).isOccupied = true;
}

void Grid::vacate(HexPoint hex) {
	if(!isValid(hex)) return;

	map.at(hex).isOccupied = false;
}

HexPoint Grid::inject(Vector2 point) {
    HexPoint hex = HexPoint(
    	(point.x - origin.x) / hexSize, 
        (point.y - origin.y) / hexSize);
    float q = view.b0 * hex.q + view.b1 * hex.r;
    float r = view.b2 * hex.q + view.b3 * hex.r;
    float s = -q - r;
    // float division needs to round to the nearest int
    int q1 = static_cast<int>(roundf(q));
    int r1 = static_cast<int>(roundf(r));
    int s1 = static_cast<int>(roundf(s));
    // after rounding we do not have a guarantee that q + r + s = 0. 
    // reset the component with the largest change back to what the constraint requires
    double q_diff = abs(q1 - q);
    double r_diff = abs(r1 - r);
    double s_diff = abs(s1 - s);
    if (q_diff > r_diff && q_diff > s_diff) {
        q1 = -r1 - s1;
    } else if (r_diff > s_diff) {
        r1 = -q1 - s1;
    } else {
        s1 = -q1 - r1;
    }

	return { q1, r1, s1 };
}

Vector2 Grid::project(HexPoint hexPoint) {
    float x = (view.f0 * hexPoint.q + view.f1 * hexPoint.r) * hexSize;
    float y = (view.f2 * hexPoint.q + view.f3 * hexPoint.r) * hexSize;
	
	return { x + origin.x, y + origin.y };
}

void Grid::resize(int width, int height) {
	hexSize = HEX_SIZE*window.unit;
	origin = { window.halfWidthf, window.halfHeightf };

	for (auto& [point, hex] : map ) {
		hex.position = project(point);
	}
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

HexPoint Grid::hexCorner(HexPoint unitHex) const {
	HexPoint maxHex = { gridSize, gridSize, gridSize };
    HexPoint corner = { unitHex.q*maxHex.q, unitHex.r*maxHex.r, unitHex.s*maxHex.s };
    if (!isValid(corner)) {
    	TraceLog(LOG_ERROR, "Did not use a unit hex for hexCorner");
    }

    return corner;
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

// heuristic algo for "rotating" a hex to get the adjacent hex
// for unit cubic coordinates, i.e. north rotates to north west and north east
HexPoint Grid::hexUnitRotate(HexPoint hex, bool clockwise) const {
	if (clockwise) {
		if (hex.q == 0) {
			return { hex.s, hex.r, hex.q };
		} else if (hex.r == 0) {
			return { hex.r, hex.q, hex.s };
		} else if (hex.s == 0) {
			return { hex.q, hex.s, hex.r };
		}
	} else {
		if (hex.q == 0) {
			return { hex.r, hex.q, hex.s };
		} else if (hex.r == 0) {
			return { hex.q, hex.s, hex.r };
		} else if (hex.s == 0) {
			return { hex.s, hex.r, hex.q };
		}		
	}

	TraceLog(LOG_ERROR, "Attempted to rotate non-unit vector.");
	return hex;
}

HexPoint Grid::hexFindFirstEmpty() const {
	HexPoint result = HexPoint(-1, -1, -1);

	for (auto& [hex, state] : map) {
		if (!state.isOccupied) {
			result = hex;
			break;
		}
	}

	return result;
}

HexPoint Grid::hexFindCenterEmpty() const {
	HexPoint result = HexPoint(-1, -1, -1);

	for (auto& [hex, state] : map) {
		if (abs(hex.q) + abs(hex.r) + abs(hex.s) <= 3 && !state.isOccupied) {
			result = hex;
			break;
		}
	}

	if (result.q == -1 && result.r == -1 && result.s == -1) {
		result = hexFindFirstEmpty();
	}

	return result;
}

HexPoint Grid::hexFindRandomEmpty() const {
	HexPoint result = HexPoint(-1, -1, -1);
	int hq = GetRandomValue(-3, 3);
	int hr = GetRandomValue(-3, 3);
	int hs = -hq - hr;

	HexPoint candidate = HexPoint(hq, hr, hs);
	bool isOccupied = true;
	bool verified = false;
	if (isValid(candidate)) {
		isOccupied = map.at(candidate).isOccupied;
	}
	int maxRetry = 15;
	while (maxRetry > 0 && isOccupied) {
		int hq = GetRandomValue(-3, 3);
		int hr = GetRandomValue(-3, 3);
		int hs = -hq - hr;
		
		candidate = HexPoint(hq, hr, hs);
		if (isValid(candidate)) {
			isOccupied = map.at(candidate).isOccupied;
			verified = !isOccupied;
		}
		maxRetry--;
	}

	if (verified) { 
		result = candidate;
	} else {
		result = hexFindCenterEmpty();
	}

	return result;
}


// TODO: use gridSize (# of layers) instead of hardcoding number here
// then subtract 1 on layers to walk through grid from a certain edge
// towards the other edge
// i.e. top to bottom: q is invariant, r subtracts 1, s adds 1
// top right to bottom left: s is invariant, q subtracts 1, r adds 1
// bottom right to top left: r is invariant, q subtracts 1, s adds 1
// etc.
// also find a way to label each corner programmatically
// then for any given direction, start at the corner
// gather the whole edge top layer, and apply above algorithm
// to walk in the opposite direction of the player moved direction

// this can be done by "columns" along the direction
// start with one of the corners, walk back, move to next column
// this algo, can be by column OR by row

// use the by row to collect all the sigils that CAN move
// use the column to move those sigils and drag the other ones
// the do another pass for merging, tagging sigils that have already merged
// NOTE: it can also be done in one pass
// NOTE: get the hex direction by getting the neighbor of 0,0,0
// then use that hex to add it to each hex in the row to walk in that direction
// that is for columns
// For rows, get the counter-clockwise adjacent neighbors of 0,0,0, and add those
// respectively, depending on the corner that you start from

// NOTE: find the pattern to programmatically rotate clockwise or counter-clockwise neighboring hexes

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

bool Grid::isHexEdge(HexPoint hex) const {
	return abs(hex.q) == gridSize || abs(hex.r) == gridSize || abs(hex.s) == gridSize;
}

Effigy Grid::getEffigy(HexPoint hex) const {
	if (!isValid(hex)) return Effigy({});

	return map.at(hex).effigy;
}

int Grid::getTotalHexes() const {
	return 1 + 6*sumCount(gridSize);
}

void Grid::clear() {
	for (auto& [hex, state] : map) {
		state.isOccupied = false;
	}
}

bool Grid::isFull() const {
	for (auto& [hex, state] : map) {
		if (!state.isOccupied) {
			return false;
		}
	}

	return true;
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