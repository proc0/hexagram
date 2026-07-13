#include "sigil.hpp"
#include "raymath.h"
#include "types.hpp"

#include <raylib.h>

void Sigil::load() {
	bgColor = getColor();
}

void Sigil::reset(HexPoint point, Effigy eff, Vector2 pos) {
	hex = point;
	effigy = eff;
	position = pos;
	lastPosition = pos;
	isAbsorbed = false;
	isMerged = false;
	frameMergeIndex = 0;
	frameMoveIndex = 0;
	state = State::Sigil::STILL;
	bgColor = getColor();
	beginEffect(Kind::SigilEffect::SPAWN);
}

// TODO: memoize this in a member var
Color Sigil::getColor() const {
	Color col = GRAY;
	switch(effigy.value) {
		case 4:
			col = SKYBLUE;
			break;
		case 8:
			col = PINK;
			break;
		case 16:
			col = LIME;
			break;
		case 32:
			col = DARKBLUE;
			break;
		case 64:
			col = VIOLET;
			break;
		case 128:
			col = DARKPURPLE;
			break;
		case 256:
			col = GOLD;
			break;
		case 512:
			col = MAGENTA;
			break;
		case 1024:
			col = RED;
			break;
		case 2048:
			col = BLACK;
			break;
		default:
			col = LIGHTGRAY;
	}

	return col;
}

void Sigil::render() const {
	DrawPoly(position, 6, sigilSize, 0.0f, bgColor);
	DrawPolyLines(position, 6, sigilSize, 0.0f, BLACK);
	const char* sigilValue = TextFormat("%d", effigy.value);
	float fontWidth = MeasureText(sigilValue, 42);
	DrawText(sigilValue, position.x-fontWidth*0.5f, position.y-21.0f, 42, WHITE);
}

std::pair<int, int> Sigil::update(const Grid& grid, Direction dir, bool isChain) {
	int mergeIndex = 0;
	int chainIndex = 0;
	
	// TraceLog(LOG_INFO, "----- Sigil %d (%d) Update -----", effigy.index, effigy.value);
	// save starting position
	HexPoint sourceHex = hex;
	// initial target hex in the moving direction
	HexPoint targetHex = grid.hexNeighbor(hex, dir);
	// TraceLog(LOG_INFO, "Target Neighbor Hex: %d %d %d", targetHex.q, targetHex.r, targetHex.s);

	// if neigbhor returns source hex, sigil cannot move
	// if (targetHex == sourceHex || isMerged || isAbsorbed) {
	if (targetHex == sourceHex || isMerged || isAbsorbed) {
		// TraceLog(LOG_INFO, "CANNOT MOVE");
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
	// if this is a chain move, do not merge
	if (grid.isOccupied(targetHex) && !isChain) {

		Effigy targetEffigy = grid.getEffigy(targetHex);
		// if the values are the same send the mergeIndex index to World
		// if this sigil was already merged into (by another sigil) do not merge
		// TODO: separate merge logic into its own method
		if (targetEffigy.value == effigy.value) {			
			// TraceLog(LOG_INFO, "Merging values: %d %d", targetEffigy.value, effigy.value);
			// update hex to new target
			hex = targetHex;
			// return target index for merging
			mergeIndex = targetEffigy.index;
		}
		// if sigil has not moved
		if (sourceHex == hex) {
			// target hex is occupied and sigil cannot move
			// sigil can still merge but moves end here
			// TraceLog(LOG_INFO, "Cannot move forward or merge");
			return std::make_pair(mergeIndex, chainIndex);
		}
	}

	// TraceLog(LOG_INFO, "Destination Hex: %d %d %d", hex.q, hex.r, hex.s);
	// update screen position
	// position = grid.hexPosition(hex);

	// get the hex behind in the opposite direction (will return sourceHex if edge)
	HexPoint chainHex = grid.hexNeighbor(sourceHex, oppositeDir(dir));
	// if the hex behind the current one (before moving)
	// contains a sigil, return the index to move it
	if (chainHex != sourceHex && grid.isOccupied(chainHex) && chainHex != grid.hexNeighbor(hex, oppositeDir(dir))) {
		// retrieve effigy to get sigil index to return to World
		Effigy chainEffigy = grid.getEffigy(chainHex);
		chainIndex = chainEffigy.index;
		// TraceLog(LOG_INFO, "Chain Hex %d (%d) at %d %d %d", chainEffigy.index, chainEffigy.value, chainHex.q, chainHex.r, chainHex.s);
	}

	// return merge sigil index to merge with this one
	// and a chain sigil to call subsequent sigil update
	return std::make_pair(mergeIndex, chainIndex);
}

void Sigil::beginMovement() {
	state = State::Sigil::MOVING;
	// animPos.source = lastPosition;
	// animPos.current = lastPosition;
	// animPos.target = position;
	// animPos.index = 0;
	// animPos.state = State::Animate::PLAYING;
	targetPosition = position;
	position = lastPosition;
	frameMoveIndex = 0;
	// TraceLog(LOG_INFO, "BEGIN ANIMATION: %f %f - %f %f ", lastPosition.x, lastPosition.y, position.x, position.y);
}

void Sigil::updateMovement() {
	if (state != State::Sigil::MOVING) return;

	if (frameMoveIndex < ANIM_FRAMES.size()) {
		// float progress = static_cast<float>(ANIM_FRAMES[animPos.index])/MAX_FRAMES;
		// animPos.current = Vector2Lerp(animPos.source, animPos.target, progress);
		position = Vector2Lerp(lastPosition, targetPosition, ANIM_FRAMES[frameMoveIndex]);
		frameMoveIndex++;
	} else {
		state = State::Sigil::STILL;
	}
}

void Sigil::beginEffect(Kind::SigilEffect kind) {
	effectState = State::Sigil::EFFECT;
	currentEffect = kind;

	if (kind == Kind::SigilEffect::SPAWN) {		
		frameEffectIndex = 0;
		currentBgColor = bgColor;
	} else if (kind == Kind::SigilEffect::MERGE) {		
		frameEffectIndex = 0;
		currentSigilSize = sigilSize;
	}
}

void Sigil::updateEffect() {
	if (effectState != State::Sigil::EFFECT) return;

	if (currentEffect == Kind::SigilEffect::SPAWN) {

		if (frameEffectIndex < ANIM_EASE_OUT_QUAD.size()) {
			Color targetColor = ColorLerp(currentBgColor, WHITE, 0.8f);
			bgColor = ColorLerp(currentBgColor, targetColor, ANIM_EASE_OUT_QUAD[frameEffectIndex]);
			frameEffectIndex++;
		} else {
			effectState = State::Sigil::STILL;
			currentEffect = Kind::SigilEffect::NORMAL;
			bgColor = currentBgColor;
		}
	} else if (currentEffect == Kind::SigilEffect::MERGE) {
		if (frameEffectIndex < ANIM_EASE_OUT_QUAD.size()) {
			float targetSize = currentSigilSize*1.2f;
			sigilSize = Lerp(currentSigilSize, targetSize, ANIM_EASE_OUT_QUAD[frameEffectIndex]);
			frameEffectIndex++;
		} else {
			effectState = State::Sigil::STILL;
			currentEffect = Kind::SigilEffect::NORMAL;
			sigilSize = currentSigilSize;
		}
	}
}

void Sigil::setAbsorbed(bool absorbed) {
	isAbsorbed = absorbed;
}

bool Sigil::hasBeenAbsorbed() const {
	return isAbsorbed;
}

void Sigil::setMerged(bool hasMerged) {
	isMerged = hasMerged;
}

bool Sigil::hasMerged() const {
	return isMerged;
}

Vector2 Sigil::getPosition() const {
	return position;
}

void Sigil::setPosition(Vector2 pos) {
	lastPosition = position;
	position = pos;
}

HexPoint Sigil::getHex() const {
	return hex;
}

void Sigil::setHex(HexPoint point) {
	hex = point;
}

Effigy Sigil::getEffigy() const {
	// if (isMerged) {
	// 	return nextEffigy;
	// }
	return effigy;
}

void Sigil::setEffigy(Effigy eff) {
		effigy = eff;
	
	// if (isMerged) {
	// 	nextEffigy = eff;
	// } else {
	// 	effigy = eff;
	// }
	bgColor = getColor();
}

void Sigil::finishMerge() {
	effigy = nextEffigy;
	bgColor = getColor();
	beginEffect(Kind::SigilEffect::MERGE);
}

bool Sigil::canMove(const Grid& grid) const {
	bool availableMove = false;
	for (auto& dir : ALL_DIRECTIONS) {
		if (!grid.isEdge(hex, dir)) {
			HexPoint neighbor = grid.hexNeighbor(hex, dir);
			if (hex != neighbor && (!grid.isOccupied(neighbor) || grid.getEffigy(neighbor).value == effigy.value)) {
				availableMove = true;
				break;
			}
		}
	}

	return availableMove;
}

bool Sigil::isMoving() const {
	return state == State::Sigil::MOVING;
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

void Sigil::resize(float unit) {
	sigilSize = SIGIL_SIZE*unit;
}

void Sigil::log(const char* context) const {
	TraceLog(LOG_INFO, "Sigil %d (%d) at (%d,%d,%d): %s", effigy.index, effigy.value, hex.q, hex.r, hex.s, context);
}