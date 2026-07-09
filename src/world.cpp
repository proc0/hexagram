#include "world.hpp"

// #include "config.h"
#include "raylib.h"
#include "types.hpp"
// #include "defaults.hpp"

// #include <string>

void World::load(){
    // std::string pathAssets = DIR_ASSETS;
    // const char* pathSoundSplat = pathAssets.append("/").append(URI_SOUND_SPLAT).c_str();

    // splat = LoadSound(pathSoundSplat);

    grid.load();

    HexPoint hex = HexPoint(0, 0, 0);
    Effigy eff = Effigy(0, 4);
    sigils.emplace_back(hex, grid.hexPosition(hex), eff);
    grid.occupy(hex, eff);
}

void World::renderUnit() const {

}

void World::renderMain() const {
    DrawRectangleGradientH(0, 0, window.width, window.height, GREEN, PINK);
}

void World::renderGame() const {
    // DrawRectangleGradientH(0, 0, window.width, window.height, BLUE, ORANGE);
    grid.render();

    for (auto& sigil : sigils) {
        sigil.render();
    }
}

void World::updateUnit(){

}

void World::updateMain(){

}

void World::updateSigils(Direction dir) {
    for (auto& sigil : sigils) {
        // TODO: sigil.update should return an index to sigils
        // if index is > 0 (TODO: add null sigil at 0) update that sigil as well.
        // NOTE: use a while loop, get the first index from the one update
        // then while (sigilIdx > 0) ... find sigil and update and store sigilIdx again
        sigil.update(grid, dir);
    }
}

void World::updateGame(){

    grid.update();

    if (IsKeyPressed(KEY_W)) {
        updateSigils(Direction::UP);
    } else if (IsKeyPressed(KEY_E)) {
        updateSigils(Direction::UP_RIGHT);
    } else if (IsKeyPressed(KEY_D)) {
        updateSigils(Direction::DOWN_RIGHT);
    } else if (IsKeyPressed(KEY_S)) {
        updateSigils(Direction::DOWN);
    } else if (IsKeyPressed(KEY_A)) {
        updateSigils(Direction::DOWN_LEFT);
    } else if (IsKeyPressed(KEY_Q)) {
        updateSigils(Direction::UP_LEFT);
    }

    //NOTES: create a vector of Sigil to iterate over
    //Sigils will have a HexPoint that links to Grid
    // For any given direction:
    // Look in the direction in front of the Sigil by getting hexNeighbor
    // if it's empty TODO: add Grid.isEmpty
    // Move Sigil forward, and then look at the neighbor in opposite direciton of movement
    // if there is a sigil behind, repeat / recurse the same algorithm until the edge of the hex grid
    // then repeate the entire algorithm until the leading sigil hits the edge or another sigil
    
    // if(IsKeyPressed(KEY_SPACE)){
    //     PlaySound(splat);
    // }
    
    // if (IsKeyPressed(KEY_H)){
    //     if (IsCursorHidden()){
    //         ShowCursor();
    //     } else {
    //         HideCursor();
    //     }
    // }
}

void World::transition(State::Screen screen) {
    switch(screen) {
        case State::Screen::MAIN:
            update = &World::updateMain;
            render = &World::renderMain;
            break;
        case State::Screen::GAME:
            update = &World::updateGame;
            render = &World::renderGame;
            break;
        default:
            update = &World::updateUnit;
            render = &World::renderUnit;
    };
}

void World::unload(){
    // UnloadSound(splat);
}

void World::resize(int width, int height) {
    TraceLog(LOG_INFO, "HELLO FROM WORLD RESIZE %i %i", width, height);
}