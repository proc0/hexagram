#pragma once

#include "types.hpp"
#include "grid.hpp"

#include <raylib.h>

#define SIGIL_SIZE 60.0f

class Sigil : public Layer {
	HexPoint hex;
	Vector2 position;

	int sigilSize = SIGIL_SIZE;
	// TODO: add Sigil types instead of value
	int value;

public:
	Sigil(HexPoint point, Vector2 pos, int val) 
		: hex(point), position(pos), value(val) {}
	~Sigil() = default;

	void load();

	void render() const;
	void update(const Grid& grid, Direction dir);

	void resize(int width, int height) override;
};
