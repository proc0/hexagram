#pragma once

#include "types.hpp"
#include "grid.hpp"

#include <raylib.h>

#define SIGIL_SIZE 60.0f

class Sigil : public Layer {
	HexPoint hex;
	Vector2 position;
	Vector2 lastPosition;
	Vector2 targetPosition;
	
	Effigy effigy;
	Effigy nextEffigy;

	int sigilSize = SIGIL_SIZE;
	State::Sigil state = State::Sigil::STILL;
	int frameMoveIndex = 0;
	int frameMergeIndex = 0;

	bool active = true;
	bool isMerged = false;
	bool isAbsorbed = false;

public:
	Sigil(HexPoint point, Effigy eff, Vector2 pos) 
		: hex(point), position(pos), lastPosition(pos), effigy(eff), nextEffigy(eff) {}
	~Sigil() = default;

	void load();
	void reset(HexPoint, Effigy, Vector2 position);

	void render() const;
	std::pair<int, int> update(const Grid& grid, Direction dir, bool isChain);
	void updateMovement();
	void beginMovement();

	Color getColor() const;

	void setAbsorbed(bool);
	bool hasBeenAbsorbed() const;

	void setMerged(bool);
	bool hasMerged() const;
	void finishMerge();

	Vector2 getPosition() const;
	void setPosition(Vector2 position);

	HexPoint getHex() const;
	void setHex(HexPoint);
	
	Effigy getEffigy() const;
	void setEffigy(Effigy);
	
	bool isMoving() const;
	bool isActive() const;
	void enable();
	void disable();
	
	void resize(int width, int height) override;
	void log(const char* context) const;
};
