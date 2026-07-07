#pragma once

#include "window.hpp"
#include "types.hpp"

#include <raylib.h>

class Grid : public Layer {
    const Window& window;

    float radius = 60.0f;
    
public:
    Grid(const Window& window): window(window) {};
    ~Grid() = default;

    // void load();
    
    void renderGrid() const;
    void drawHex(Vector2 position) const;

    void updateGrid();
    

    void resize(int width, int height) override;
    // void transition(State::Screen);
    // void unload();
};
