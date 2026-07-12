#pragma once

#include "window.hpp"
#include "types.hpp"
#include "grid.hpp"
#include "sigil.hpp"

class World : public Layer {
    std::vector<Sigil> sigils;

    Color phaseColor1 = RAYWHITE;
    Color phaseColor2 = BLUE;

    const Window& window;
    Grid grid = Grid(window);

    State::World state = State::World::WAITING;
    int maxSigilValue = 2;

public:
    World(const Window& window): window(window) {};
    ~World() = default;
    
    void (World::*render)() const = &World::renderUnit;
    void (World::*update)() = &World::updateUnit;

    void load();
    void restart();

    void renderUnit() const;
    void renderMain() const;
    void renderGame() const;

    void updateUnit();
    void updateMain();
    void updateGame();
    void updateSigils(Direction dir);
    void createSigil(HexPoint, int value);
    void placeSigil(int index);
    void spawnSigil(int value);
    void changePhase();
    
    int getRandomSigilValue() const;

    bool isMoveAvailable() const;
    bool isGridLocked() const;
    bool isGridFull() const;
    bool isMaxSigilValue(int value) const;
    void resize(int width, int height) override;
    void transition(State::Screen);
    void unload();
};
