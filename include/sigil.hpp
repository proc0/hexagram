#pragma once

#include "types.hpp"
#include "grid.hpp"

#include <raylib.h>

#define SIGIL_SIZE 60.0f

class Sigil : public Layer {
	HexPoint hex;
	Vector2 position;

	Effigy effigy;
	
	int sigilSize = SIGIL_SIZE;

public:
	Sigil(HexPoint point, Vector2 pos, Effigy eff) 
		: hex(point), position(pos), effigy(eff) {}
	~Sigil() = default;

	void load();

	void render() const;
	void update(const Grid& grid, Direction dir);

	void resize(int width, int height) override;
};
