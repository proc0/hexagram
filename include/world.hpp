#pragma once

#include "window.hpp"
#include "types.hpp"
#include "grid.hpp"
#include "sigil.hpp"

class World : public Layer {
    std::vector<Sigil> sigils;

    Color phaseColor1 = ColorLerp(BLUE, WHITE, 0.5f);
    Color phaseColor2 = ColorLerp(GREEN, WHITE, 0.5f);

    Window& window;
    Grid grid = Grid(window);

    // float phaseColorLerp = 0.5f;
    State::World state = State::World::WAITING;
    int score = 2;
    int maxSigilValue = 2;
    bool phaseChangeEvent = false;
    // bool phaseColorLerpSwitch = false;

public:
    World(Window& window): window(window) {};
    ~World() = default;
    
    void (World::*render)() const = &World::renderUnit;
    void (World::*update)(Action::Surface) = &World::updateUnit;

    void load();
    void restart();

    void renderUnit() const;
    void renderMain() const;
    void renderGame() const;

    void updateUnit(Action::Surface);
    void updateMain(Action::Surface);
    void updateGame(Action::Surface);
    void updateSigils(Direction dir);
    void createSigil(HexPoint, int value);
    void placeSigil(int index);
    void spawnSigil(int value);
    void phaseChange();

    int getScore() const;
    int getRandomSigilValue() const;
    int getMaxSigilValue() const;

    bool isMoveAvailable() const;
    bool isGridLocked() const;
    bool isGridFull() const;
    bool isMaxSigilValue(int value) const;
    void resize(int width, int height) override;
    void transition(State::Screen);
    void unload();
};
