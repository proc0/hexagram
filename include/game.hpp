#pragma once

#include "types.hpp"
#include "defaults.hpp"
#include "window.hpp"
#include "world.hpp"

#include <raylib.h>

class Game : public Layer {
    const char* raylibName = "raylib";
    const int raylibLogoFontSize = 40;
    const float raylibLogoSize = 200;
    const int raylibLogoBorder = 16;
    const float raylibLogoInnerSize = raylibLogoSize - 2.0f*raylibLogoBorder;
    Vector2 raylibLogoDir = { static_cast<float>(GetRandomValue(-100, 100))/100.0f, static_cast<float>(GetRandomValue(-100, 100))/100.0f };
    Vector2 raylibLogoPos = { static_cast<float>(GetRandomValue(0, SCREEN_WIDTH-raylibLogoSize)), static_cast<float>(GetRandomValue(0, SCREEN_HEIGHT-raylibLogoSize)) };
    Rectangle raylibLogoOuterRec;
    Rectangle raylibLogoInnerRec;
    Vector2 raylibLogoTextPos;

    GameState gameState = {
        .state = State::Game::START
    };

    const Window& window;
    const World& world;
    State::Game state = State::Game::START;

public:
    Game(const Window& window, const World& world): window(window), world(world) {}
    ~Game() = default;

    void load();
    void loadRaylibLogo();
    void restart();

    void renderNull() const;
    void (Game::*render)() const = &Game::renderNull;
    void renderGame() const;
    void renderMain() const;
    void renderRaylibLogo() const;

    GameState updateNull(State::App, Action::Surface, InputEvent);
    GameState (Game::*update)(State::App, Action::Surface, InputEvent) = &Game::updateNull;
    GameState updateMain(State::App, Action::Surface, InputEvent);
    GameState updateGame(State::App, Action::Surface, InputEvent);
    void updateRaylibLogo();

    bool isOver() const;
    void transition(State::Screen);
    void resize(int height, int width) override;
    void unload();
};
