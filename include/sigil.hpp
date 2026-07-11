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
	bool active = true;
public:
	Sigil(HexPoint point, Vector2 pos, Effigy eff) 
		: hex(point), position(pos), effigy(eff) {}
	~Sigil() = default;

	void load();

	Color getColor() const;
	void render() const;
	std::pair<int, int> update(const Grid& grid, Direction dir);

	Vector2 getPosition() const;
	void setPosition(Vector2 position);
	HexPoint getHex() const;
	void setHex(HexPoint);
	Effigy getEffigy() const;
	void setEffigy(Effigy);
	bool isActive() const;
	void enable();
	void disable();
	void resize(int width, int height) override;
	void log(const char* context) const;
};
