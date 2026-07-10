#pragma once

#include "window.hpp"
#include "types.hpp"
#include "grid.hpp"
#include "sigil.hpp"

class World : public Layer {
    std::vector<Sigil> sigils;

    const Window& window;
    Grid grid = Grid(window);

public:
    World(const Window& window): window(window) {};
    ~World() = default;
    
    void (World::*render)() const = &World::renderUnit;
    void (World::*update)() = &World::updateUnit;

    void load();
    
    void renderUnit() const;
    void renderMain() const;
    void renderGame() const;

    void updateUnit();
    void updateMain();
    void updateGame();
    void updateSigils(Direction dir);
    void placeSigil(HexPoint, int value);
    void spawnSigil(int value);

    void resize(int width, int height) override;
    void transition(State::Screen);
    void unload();
};
