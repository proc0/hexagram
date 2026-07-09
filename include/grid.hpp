#pragma once

#include "window.hpp"
#include "types.hpp"

#include <raylib.h>

class Grid : public Layer {

  const Window& window;

public:
    Grid(const Window& window): window(window) {};
    ~Grid() = default;

    void load();
    
    void renderGrid() const;
    void updateGrid();

    void resize(int width, int height) override;
    // void transition(State::Screen);
    // void unload();
};
