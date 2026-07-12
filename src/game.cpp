#include "game.hpp"

#include "types.hpp"
#include <raylib.h>

void Game::load() {
    loadRaylibLogo();
}

void Game::restart() {
    state = State::Game::PLAY;
    gameState.state = state;
}

void Game::loadRaylibLogo() {

    float raylibLogoX = window.halfWidthf-raylibLogoSize*0.5f;
    float raylibLogoY = static_cast<float>(window.height-raylibLogoSize-20);
    float raylibLogoInnerX = raylibLogoX + raylibLogoBorder;
    float raylibLogoInnerY = raylibLogoY + raylibLogoBorder;

    int raylibLogoTextSize = MeasureText(raylibName, raylibLogoFontSize);
    float raylibLogoTextX = raylibLogoX + raylibLogoSize - raylibLogoTextSize - 2.0f*raylibLogoBorder;
    float raylibLogoTextY = raylibLogoY + raylibLogoSize - raylibLogoFontSize - 1.5f*raylibLogoBorder;

    raylibLogoOuterRec = { raylibLogoX, raylibLogoY, raylibLogoSize, raylibLogoSize };
    raylibLogoInnerRec = { raylibLogoInnerX, raylibLogoInnerY, raylibLogoInnerSize, raylibLogoInnerSize };
    raylibLogoTextPos = { raylibLogoTextX, raylibLogoTextY };
}

void Game::updateRaylibLogo() {
    raylibLogoPos.x += raylibLogoDir.x;
    raylibLogoPos.y += raylibLogoDir.y;

    // bool hasBounced = false;
    if (raylibLogoPos.x < 0 || raylibLogoPos.x + raylibLogoOuterRec.width > window.width) {
        raylibLogoDir.x *= -1.0f;
        // gameState.raylibLogoBounces++;
        // hasBounced = true;
    } 

    if (raylibLogoPos.y < 0 || raylibLogoPos.y + raylibLogoOuterRec.width > window.height) {
        raylibLogoDir.y *= -1.0f;
        // gameState.raylibLogoBounces++;
        // if (hasBounced) {
        //     gameState.raylibLogoCorners++;
        // }
    }

    float raylibLogoInnerX = raylibLogoPos.x + raylibLogoBorder;
    float raylibLogoInnerY = raylibLogoPos.y + raylibLogoBorder;
    
    int raylibLogoTextSize = MeasureText(raylibName, raylibLogoFontSize);
    float raylibLogoTextX = raylibLogoPos.x + raylibLogoSize - raylibLogoTextSize - 2.0f*raylibLogoBorder;
    float raylibLogoTextY = raylibLogoPos.y + raylibLogoSize - raylibLogoFontSize - 1.5f*raylibLogoBorder;

    raylibLogoOuterRec = { raylibLogoPos.x, raylibLogoPos.y, raylibLogoSize, raylibLogoSize };
    raylibLogoInnerRec = { raylibLogoInnerX, raylibLogoInnerY, raylibLogoInnerSize, raylibLogoInnerSize };
    raylibLogoTextPos = { raylibLogoTextX, raylibLogoTextY };
}

GameState Game::updateNull(State::App appState, Action::Surface action, InputEvent inputEvent){
    return gameState;
}

GameState Game::updateMain(State::App appState, Action::Surface action, InputEvent inputEvent){
    return gameState;
}

GameState Game::updateGame(State::App appState, Action::Surface action, InputEvent inputEvent){

    if (world.isGridLocked()) {
        state = State::Game::OVER;
        gameState.state = state;
    }

    if (action == Action::Surface::RESTART_GAME) {
        restart();
    }

    if (world.getMaxSigilValue() >= 2048) {
        state = State::Game::WIN;
        gameState.state = state;
    }

    gameState.score = world.getScore();

    return gameState;
}

void Game::renderRaylibLogo() const {
    DrawRectangleRec(raylibLogoOuterRec, BLACK);
    DrawRectangleRec(raylibLogoInnerRec, RAYWHITE);
    DrawText(raylibName, raylibLogoTextPos.x, raylibLogoTextPos.y, raylibLogoFontSize, BLACK);
}

void Game::renderNull() const {

}

void Game::renderMain() const {

}

void Game::renderGame() const {
    // renderRaylibLogo();
}

bool Game::isOver() const {
    return state == State::Game::OVER;
}

void Game::transition(State::Screen screen) {
    switch(screen) {
        case State::Screen::MAIN:
            update = &Game::updateMain;
            render = &Game::renderMain;

            state = State::Game::START;
            gameState.state = state;
            break;
        case State::Screen::GAME:
            update = &Game::updateGame;
            render = &Game::renderGame;

            state = State::Game::PLAY;
            gameState.state = state;
            break;
        default:
            update = &Game::updateNull;
    };
}

void Game::resize(int height, int width) {
    // loadRaylibLogo();
}

void Game::unload(){

}
