#include "grid.hpp"
// #include "macros.hpp"

#include <raylib.h>
#include <raymath.h>
// #include <stdexcept>

void Grid::load() {
	// float originX = window.halfWidthf;
	// float originY = window.halfHeightf;
	// Vector2 origin = Vector2({ originX, originY });

	// Vector2 hexCenter = Vector2({ 0.0f, 0.0f });
	// drawHex(Vector2Add(hexCenter, origin));

	// number of onion layers on the center hex
	int layers = 3;
	// total number of hexes
	// hexes.reserve(1 + 6*sumCount(layers));
	// hexes.reserve(2*layers + 1);
	
	generateMap(layers);

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

	sigils.reserve(1 + 6*sumCount(layers));
	sigils.push_back(Sigil({Vector2({ 0.0f, 0.0f }), Vector2({ 0.0f, 0.0f }), 0, 0, SigilState::IDLE}));
	
	sigils.push_back(Sigil({Vector2({ 2.0f, -3.0f }), project({ 2.0f, -3.0f }), 1, 2, SigilState::IDLE}));
	hexMap.at(Vector2({ 2.0f, -3.0f })).sigilIndex = 1;
	
	sigils.push_back(Sigil({Vector2({ 2.0f, 0.0f }), project({ 2.0f, 0.0f }), 2, 2, SigilState::IDLE}));
	hexMap.at(Vector2({ 2.0f, 0.0f })).sigilIndex = 2;

	sigils.push_back(Sigil({Vector2({ 2.0f, 1.0f }), project({ 2.0f, 1.0f }), 3, 8, SigilState::IDLE}));
	hexMap.at(Vector2({ 2.0f, 1.0f })).sigilIndex = 3;
}

void Grid::generateMap(int layers) {
	for (int q = -layers; q <= layers; q++) {
	    int r1 = fmax(-layers, -q - layers);
	    int r2 = fmin( layers, -q + layers);
	    bool isColEdge = q == -layers || q == layers;
	    for (int r = r1; r <= r2; r++) {
	    	Vector2 axial = Vector2({ static_cast<float>(q), static_cast<float>(r) });
	    	// TraceLog(LOG_INFO, "generating map at (%d, %d)", q, r);
	    	bool isRowEdge = r == r1 || r == r2;
			hexMap[axial] = { axial, project(axial), isColEdge || isRowEdge, getHexEdgeType(axial), 0 };
	    }
	}
}

// void Grid::generateArray(int layers) {
// 	for (int q = -layers; q <= layers; q++) {
// 	    int r1 = fmax(-layers, -q - layers);
// 	    int r2 = fmin( layers, -q + layers);

// 	    int ri = q + layers;
// 	    hexesArray.push_back({});
//     	hexesArray.at(ri).reserve(r2 - r1 + 1);
	    
// 	    for (int r = r1; r <= r2; r++) {
// 	    	TraceLog(LOG_INFO, "HEX: %d: %d, %d", ri, q, r);
// 	    	Vector2 axial = Vector2({ static_cast<float>(q), static_cast<float>(r) });
// 			hexesArray.at(ri).emplace_back(axial, project(axial), false);
// 	    }
// 	}
// }

void Grid::renderGrid() const {

	// render hexArray
	// for (auto& hexRow : hexes) {
	// 	for (const Hex& hex : hexRow) {
	// 		drawHex(hex.projection, hex.isClicked);
	// 	}
	// }

	//render hexMap
	for (auto& [axial, hex] : hexMap) {
		drawHex(hex);
	}

	for (const Sigil& sigil : sigils) {
		drawSigil(sigil);
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

	if (IsKeyPressed(KEY_W)) {
		TraceLog(LOG_INFO, "MOVE UP");
		if(sigilState == SigilState::IDLE) {
			sigilState = SigilState::MOVING;
			// TODO: remove this once its safe
			int safeguard = 1000;

			// look at the sigil neighbors to determine who to move,
			// then look at the opposite direction to check if there was a sigil 
			// and move that, repeating until there are no more sigil neighbors
			for (int i = 1; i < sigils.size(); i++) {

				Sigil& sourceSigil = sigils[i];
				sourceSigil.state = SigilState::MOVING;
				// TraceLog(LOG_INFO, "SIGIL POSITION: %f %f", sourceSigil.position.x, sourceSigil.position.y);
				Vector2 sourceAxial = sourceSigil.position;

				TraceLog(LOG_INFO, "Evaluating Sigil");

				// if the current hex location of the sigil isn't one of the edges in the direction pressed
				// then walk hex by hex in that direction checking if the hex is clear for movement
				// do this until another hex with a sigil is reached or an edge is reached
				if (!checkHexEdgeType(HexEdgeType::TOP_RIGHT, sourceAxial) && !checkHexEdgeType(HexEdgeType::TOP_LEFT, sourceAxial)) {
					// next hex position, one hex in the direction of the current sigil
					// TODO: abstract this to move in any direction
					Vector2 targetAxial = getNeighborAxial(Directions::DIR_UP, sourceAxial, 1.0f);
					TraceLog(LOG_INFO, "Checking initial target Hex");

					//get neighbor sigil in the direction of the move, and if it's blocked, skip
					Hex& targetHex = hexMap.at(targetAxial);
					if (targetHex.sigilIndex != 0) {
						TraceLog(LOG_INFO, "Initial target hex is occupied by sigil");
						// the next hex has a sigil, so this sigil cannot move
						// it will be pulled in the chain if the sigil on top can move
						continue;
					}

					TraceLog(LOG_INFO, "Calculating final destination hex of initial sigil");

					// TraceLog(LOG_INFO, "Next Hex Edge Type: %i", targetHex.edgeType);
					// continue to iterate if the next hex has no sigil, and it is not an edge in the direction moved
					// TODO: abstract this for any direction
					bool continueMove = targetHex.sigilIndex == 0 && !checkHexEdgeType(HexEdgeType::TOP_RIGHT, targetAxial) && !checkHexEdgeType(HexEdgeType::TOP_LEFT, targetAxial);

					// iterate over the targetAxial coordinate until a stopping point
					while (safeguard > 0 && continueMove) {
						// update next axial coordinate in the direction of movement
						// TODO: abstract this for any direction
						targetAxial = getNeighborAxial(Directions::DIR_UP, targetAxial, 1.0f);
						Hex& newTargetHex = hexMap.at(targetAxial);

						// TODO: abstract this for any direction
						continueMove = newTargetHex.sigilIndex == 0 && !checkHexEdgeType(HexEdgeType::TOP_RIGHT, targetAxial) && !checkHexEdgeType(HexEdgeType::TOP_LEFT, targetAxial);

						safeguard--;
					}

					TraceLog(LOG_INFO, "Found final destination hex of initial sigil");

					// when iteration stops, the targetAxial will be the destination axial
					// get the destination hex from the destination axial
					Hex& finalTargetHex = hexMap.at(targetAxial);

					// gather all the sigils that were in the opposite direction of the source sigil
					// creating a chain that will move right below the target hex where the target sigil lands
					std::vector<Vector2> chainAxials = {};

					// process any neighbor sigils that are in the opposite direction of the move
					// moving them one down or merging them, first check the opposite direction is not an edge (relative to the opposite direction)
					if (!checkHexEdgeType(HexEdgeType::BOTTOM_RIGHT, sourceAxial) && !checkHexEdgeType(HexEdgeType::BOTTOM_LEFT, sourceAxial)) {
						TraceLog(LOG_INFO, "Gathering sigil chain below source sigil");

						Vector2 belowSourceAxial = getNeighborAxial(Directions::DIR_DOWN, sourceAxial, 1.0f);
						// get neighbor sigil in the direction of the move, and if it's blocked, skip
						Hex& belowSourceHex = hexMap.at(belowSourceAxial);
						bool continueChainCollect = belowSourceHex.sigilIndex > 0;

						while (safeguard > 0 && continueChainCollect) {
							chainAxials.push_back(belowSourceAxial);
							TraceLog(LOG_INFO, "Added 1 sigil to chain: (%f, %f)", belowSourceAxial.x, belowSourceAxial.y);

							Hex& currentBelowSourceHex = hexMap.at(belowSourceAxial);
							if (currentBelowSourceHex.sigilIndex == 0 || checkHexEdgeType(HexEdgeType::BOTTOM_RIGHT, belowSourceAxial) || checkHexEdgeType(HexEdgeType::BOTTOM_LEFT, belowSourceAxial)) {
								TraceLog(LOG_INFO, "Finished chain collecting at: (%f, %f)", belowSourceAxial.x, belowSourceAxial.y);
								break;
							}
							// move to next below hex
							belowSourceAxial = getNeighborAxial(Directions::DIR_DOWN, belowSourceAxial, 1.0f);
							TraceLog(LOG_INFO, "Checking next sigil in chain: (%f, %f)", belowSourceAxial.x, belowSourceAxial.y);
							// check if need to continue
							Hex& nextBelowSourceHex = hexMap.at(belowSourceAxial);
							continueChainCollect = nextBelowSourceHex.sigilIndex > 0;
							safeguard--;
						}

						TraceLog(LOG_INFO, "TOTAL CHAIN NUM: %d", chainAxials.size());

						// process chain sigils
						if (!chainAxials.empty()) {
							float destinationOffset = 1.0f;
							for (auto& ax : chainAxials) {
								Hex& chainHex = hexMap.at(ax);
								Sigil& chainSig = sigils.at(chainHex.sigilIndex);
								chainSig.position = getNeighborAxial(Directions::DIR_DOWN, targetAxial, destinationOffset);
								//vacate the hex
								chainHex.sigilIndex = 0;

								Hex& targetChainHex = hexMap.at(chainSig.position);
								chainSig.projection = targetChainHex.projection;
								targetChainHex.sigilIndex = chainSig.index;

								destinationOffset++;
								TraceLog(LOG_INFO, "Processed chain sigil: %d", chainSig.value);
							}
						}

						// if (belowSourceHex.sigilIndex > 0) {
						// 	Sigil& prevSigil = sigils.at(belowSourceHex.sigilIndex);
						// 	prevSigil.position = { finalTargetHex.position.x, finalTargetHex.position.y + 1.0f };

						// 	Hex& belowHex = hexMap.at(prevSigil.position);
						// 	prevSigil.projection = belowHex.projection;
							
						// 	belowHex.sigilIndex = prevSigil.index;
						// }
					}

					// update the leading sigil to the destination
					Hex& sourceHex = hexMap.at(sourceAxial);
					// vacate the initial source hex
					sourceHex.sigilIndex = 0;
					// update the source sigil positions
					sourceSigil.position = finalTargetHex.position;
					sourceSigil.projection = finalTargetHex.projection;
					// occupy the final target hex with sigil index
					finalTargetHex.sigilIndex = i;


				}
			}
		}
	}
	// Vector2 mousePos = GetMousePosition();
	// Vector2 hexAxial = inject(mousePos);

	// TraceLog(LOG_INFO, "hexAxial: %f %f", hexAxial.x, hexAxial.y);
	// try {

	// 	if (hexAxial.x >= -3.0f && hexAxial.y >= -3.0f && hexAxial.x <= 3.0f && hexAxial.y <= 3.0f) {		
	// 		if (!hexMap.at(hexAxial).isClicked) {
	// 			hexMap.at(hexAxial).isClicked = true;
	// 		}
	// 	}
	// } catch(std::out_of_range error) {
	// 	TraceLog(LOG_INFO, "hexAxial: %f %f", hexAxial.x, hexAxial.y);
	// }

	// int hexCol = hexAxial.x + 3;
	// int hexRow = hexCol > 3 ? hexAxial.y + hexCol - hexAxial.x : hexAxial.y + hexCol;

	// if (!hexes.at(hexCol).empty()) {
	// 	if(!hexes.at(hexCol).at(hexRow).isClicked) {
	// 		TraceLog(LOG_INFO, "MOUSE POS: %f %f \n HEX: %d %d", mousePos.x, mousePos.y, hexCol, hexRow);
	// 		hexes.at(hexCol).at(hexRow).isClicked = true;
	// 	}
	// 	// for (Hex& hex : hexes.at(hexRow)) {
	// 	// 	if (!hex.isClicked) {
	// 	// 		TraceLog(LOG_INFO, "MOUSE POS: %f %f \n HEX ROW: %d", mousePos.x, mousePos.y, hexRow);
	// 	// 		hex.isClicked = true;
	// 	// 	}
	// 	// }

	// }
	// for (auto& hexRow : hexes) {
	// 	for (Hex& hex : hexRow) {
	// 		if (CheckCollisionPointCircle(GetMousePosition(), hex.projection, radius)) {
	// 			hex.isClicked = true;
	// 		} else {
	// 			hex.isClicked = false;
	// 		}
	// 	}
	// }
}

void Grid::drawHex(const Hex& hex) const {
    DrawPoly(hex.projection, 6, radius, 0.0f, hex.isEdge ? YELLOW : BEIGE);
    DrawPolyLines(hex.projection, 6, radius, 0.0f, BLACK);
    const char* sigilValue = TextFormat("(%d, %d)", static_cast<int>(hex.position.x), static_cast<int>(hex.position.y));
    DrawText(sigilValue, hex.projection.x-30.0f, hex.projection.y, 20, BLACK);
}

void Grid::drawSigil(const Sigil& sigil) const {
    DrawPoly(sigil.projection, 6, radius, 0.0f, PURPLE);
    DrawPolyLines(sigil.projection, 6, radius, 0.0f, BLACK);
    const char* sigilValue = TextFormat("%d", sigil.value);
    DrawText(sigilValue, sigil.projection.x, sigil.projection.y, 20, BLACK);
}

HexEdgeType Grid::getHexEdgeType(Vector2 axial) {
	HexEdgeType edgeType = HexEdgeType::OTHER;
	// TODO: abstract over direction
	for (const Vector2& v : topRightEdgeHexes) {
		if (v.x == axial.x && v.y == axial.y) {
			edgeType = HexEdgeType::TOP_RIGHT;
			return edgeType;
		}
	}

	for (const Vector2& v : rightEdgeHexes) {
		if (v.x == axial.x && v.y == axial.y) {
			edgeType = HexEdgeType::RIGHT;
			return edgeType;
		}
	}

	for (const Vector2& v : bottomRightEdgeHexes) {
		if (v.x == axial.x && v.y == axial.y) {
			edgeType = HexEdgeType::BOTTOM_RIGHT;
			return edgeType;
		}
	}

	for (const Vector2& v : bottomLeftEdgeHexes) {
		if (v.x == axial.x && v.y == axial.y) {
			edgeType = HexEdgeType::BOTTOM_LEFT;
			return edgeType;
		}
	}

	for (const Vector2& v : leftEdgeHexes) {
		if (v.x == axial.x && v.y == axial.y) {
			edgeType = HexEdgeType::LEFT;
			return edgeType;
		}
	}

	for (const Vector2& v : topLeftEdgeHexes) {
		if (v.x == axial.x && v.y == axial.y) {
			edgeType = HexEdgeType::TOP_LEFT;
			return edgeType;
		}
	}

	return edgeType;
}

bool Grid::checkHexEdgeType(HexEdgeType edgeType, Vector2 axial) const {
	bool isMatch = false;

	switch(edgeType) {
		case HexEdgeType::TOP_RIGHT:
			isMatch = axial.y == -3.0f;
			break;
		case HexEdgeType::RIGHT:
			isMatch = axial.x == 3.0f;
			break;
		case HexEdgeType::BOTTOM_RIGHT:
			isMatch = (axial.x == 3.0f && axial.y == 0.0f) 
				|| (axial.x == 2.0f && axial.y == 1.0f)
				|| (axial.x == 1.0f && axial.y == 2.0f)
				|| (axial.x == 0.0f && axial.y == 3.0f);
			break;
		case HexEdgeType::BOTTOM_LEFT:
			isMatch = axial.y == 3.0f;
			break;
		case HexEdgeType::LEFT:
			isMatch = axial.x == -3.0f;
			break;
		case HexEdgeType::TOP_LEFT:
			isMatch = (axial.x == -3.0f && axial.y == 0.0f) 
				|| (axial.x == -2.0f && axial.y == -1.0f)
				|| (axial.x == -1.0f && axial.y == -2.0f)
				|| (axial.x == 0.0f && axial.y == -3.0f);
			break;
		default:
			isMatch = false;
	}
	return isMatch;
}

Vector2 Grid::getNeighborAxial(Directions dir, Vector2 axial, float offset) const {
	Vector2 result = axial;

	switch (dir) {
	case DIR_UP:
		result = { axial.x, axial.y - 1.0f*offset };
		break;
	case DIR_UP_RIGHT:
		break;
	case DIR_RIGHT:
		break;
	case DIR_DOWN_RIGHT:
		break;
	case DIR_DOWN:
		result = { axial.x, axial.y + 1.0f*offset };
		break;
	case DIR_DOWN_LEFT:
		break;
	case DIR_LEFT:
		break;
	case DIR_UP_LEFT:
		break;
	default:
		result = axial;
	}

	return result;
}

Vector2 Grid::roundHex(Vector2 v) {
// Hex hex_round(FractionalHex h) {
//     int q = int(round(h.q));
//     int r = int(round(h.r));
//     int s = int(round(h.s));
//     double q_diff = abs(q - h.q);
//     double r_diff = abs(r - h.r);
//     double s_diff = abs(s - h.s);
//     if (q_diff > r_diff and q_diff > s_diff) {
//         q = -r - s;
//     } else if (r_diff > s_diff) {
//         r = -q - s;
//     } else {
//         s = -q - r;
//     }
//     return Hex(q, r, s);
// }
    float q = roundf(v.x);
    float r = roundf(v.y);
    // int s = int(round(h.s));
    // double q_diff = abs(q - v.x);
    // double r_diff = abs(r - v.y);
    // double s_diff = abs(s - h.s);
    // if (q_diff > r_diff and q_diff > s_diff) {
    //     q = -r - s;
    // } else if (r_diff > s_diff) {
    //     r = -q - s;
    // } else {
    //     s = -q - r;
    // }
    return Vector2({ q, r });
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
    const Orientation& M = layout.orientation;
    Vector2 pt = Vector2((position.x - layout.origin.x) / layout.size.x, 
                     (position.y - layout.origin.y) / layout.size.y);
    float q = M.b0 * pt.x + M.b1 * pt.y;
    float r = M.b2 * pt.x + M.b3 * pt.y;

	return roundHex({ q, r });
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
