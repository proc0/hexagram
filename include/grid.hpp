#pragma once

#include "window.hpp"
#include "types.hpp"

#include <raylib.h>

typedef struct Hex {
	Vector2 position;
	Vector2 projection;
	bool isClicked;
} Hex;

class Grid : public Layer {
	std::vector<Hex> hexes = {};

    const Window& window;

    float radius = 60.0f;

public:
    Grid(const Window& window): window(window) {};
    ~Grid() = default;

    void load();
    
    void renderGrid() const;
    void drawHex(Vector2 position, bool isClicked) const;

    void updateGrid();
    

    void resize(int width, int height) override;
    // void transition(State::Screen);
    // void unload();
};
