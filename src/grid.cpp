#include "grid.hpp"
#include "macros.hpp"

#include <raylib.h>
#include <raymath.h>

void Grid::load() {
	// float originX = window.halfWidthf;
	// float originY = window.halfHeightf;
	// Vector2 origin = Vector2({ originX, originY });

	// Vector2 hexCenter = Vector2({ 0.0f, 0.0f });
	// drawHex(Vector2Add(hexCenter, origin));

	// number of onion layers on the center hex
	int layers = 3;
	// total number of hexes
	hexes.reserve(1 + 6*sumCount(layers));
	generate(layers);

	// hexes.emplace_back(Vector2({ 0.0f, 0.0f }), origin, false);
	// // drawHex(Vector2Add(hexPos, origin));
	// for (int i=0; i<6; ++i) {		
	// 	Vector2 hexPos = Vector2({ 0.0f, radius*-SQRT3_OVER_2*2.0f });
	// 	hexPos = Vector2Rotate(hexPos, RAD_60*i);
	// 	Vector2 hexProj = Vector2Add(hexPos, origin);
	// 	hexes.emplace_back(hexPos, hexProj, false);

	// 	Vector2 hexPos2 = hexPos*2.0f;
	// 	Vector2 hexProj2 = Vector2Add(hexPos2, origin);
	// 	hexes.emplace_back(hexPos2, hexProj2, false);

	// 	Vector2 hexPos3 = Vector2Rotate(hexPos, RAD_60);
	// 	Vector2 hexProj3 = Vector2Add(Vector2Add(hexPos3, hexPos), origin);
	// 	hexes.emplace_back(hexPos3, hexProj3, false);
	// }

	// hexes.emplace_back(Vector2({ 0.0f, 0.0f }), project(Vector2({ 0.0f, 0.0f })), false);
	// hexes.emplace_back(Vector2({ 1.0f, 0.0f }), project(Vector2({ 1.0f, 0.0f })), false);
	// hexes.emplace_back(Vector2({ 0.0f, 1.0f }), project(Vector2({ 0.0f, 1.0f })), false);

}

void Grid::generate(int layers) {
	for (int q = -layers; q <= layers; q++) {
	    int r1 = fmax(-layers, -q - layers);
	    int r2 = fmin( layers, -q + layers);
	    for (int r = r1; r <= r2; r++) {
	    	Vector2 axial = Vector2({ static_cast<float>(q), static_cast<float>(r) });
			hexes.emplace_back(axial, project(axial), false);
	    }
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

Vector2 Grid::inject(Vector2 position) {
// FractionalHex pixel_to_hex_fractional(Layout layout, Point p) {
//     const Orientation& M = layout.orientation;
//     Point pt = Point((p.x - layout.origin.x) / layout.size.x, 
//                      (p.y - layout.origin.y) / layout.size.y);
//     double q = M.b0 * pt.x + M.b1 * pt.y;
//     double r = M.b2 * pt.x + M.b3 * pt.y;
//     return FractionalHex(q, r, -q - r);
// }
	return {};
}

Vector2 Grid::project(Vector2 axial) {
// Point hex_to_pixel(Layout layout, Hex h) {
//     const Orientation& M = layout.orientation;
//     double x = (M.f0 * h.q + M.f1 * h.r) * layout.size.x;
//     double y = (M.f2 * h.q + M.f3 * h.r) * layout.size.y;
//     return Point(x + layout.origin.x, y + layout.origin.y);
// }
    const Orientation& M = layout.orientation;
    float x = (M.f0 * axial.x + M.f1 * axial.y) * layout.size.x;
    float y = (M.f2 * axial.x + M.f3 * axial.y) * layout.size.y;
	
	return { x + layout.origin.x, y + layout.origin.y };
}

void Grid::resize(int width, int height) {
	
}
