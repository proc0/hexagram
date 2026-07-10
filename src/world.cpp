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

    sigils.reserve(grid.getTotalHexes());

    // sentinel sigil
    HexPoint hex = HexPoint(0, 0, 0);
    Effigy eff = Effigy(0, 0);
    sigils.emplace_back(hex, Vector2({}), eff);
    sigils.at(0).disable();

    // placeSigil(HexPoint(0, 0, 0), 4);
    // placeSigil(HexPoint(0, 2, -2), 4);
    // placeSigil(HexPoint(0, 3, -3), 12);
    // placeSigil(HexPoint(-2, 0, 2), 2);
    // placeSigil(HexPoint(-2, 1, 1), 2);
    // placeSigil(HexPoint(3, -2, -1), 2);
    // placeSigil(HexPoint(3, 0, -3), 2);
    // placeSigil(HexPoint(-2, 3, -1), 8);
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
        if (sigil.isActive()) {
            sigil.render();
        }
    }
}

void World::updateUnit(){

}

void World::updateMain(){

}

void World::updateSigils(Direction dir) {
    for (auto& sigil : sigils) {
        if (sigil.isActive()) {
            HexPoint currHex = sigil.getHex();
            // call sigil update, and collect any sigil in front to merge
            // and any sigil behind to start the chain move
            std::pair<int, int> result = sigil.update(grid, dir);

            // merge sigils
            if (result.first > 0) {
                Sigil& mergeSigil = sigils.at(result.first);
                // mergeSigil.disable();
                
                Effigy mergeEff = mergeSigil.getEffigy();
                Effigy currEff = sigil.getEffigy();
                
                // merge the sigil that the current sigil landed on
                // the underlying sigil "absorbs" this one
                TraceLog(LOG_INFO, "Merging %d (%d) and %d (%d)", currEff.index, currEff.value, mergeEff.index, mergeEff.value);
                
                // TODO: remove this dead code if strategy works
                // this is when current sigil "lands on" underlying sigil and replaces it
                // Effigy newEff = { currEff.index, currEff.value + mergeEff.value };
                
                // tombstone sigil
                sigil.disable();
                // create new effigy with the underlying sigil's index and combined values
                Effigy newEff = { mergeEff.index, currEff.value + mergeEff.value };
                mergeSigil.setEffigy(newEff);
                // update grid effigy
                grid.occupy(mergeSigil.getHex(), mergeSigil.getEffigy());
            }

            // if the sigil moved, update grid
            if (currHex != sigil.getHex()) {
                // remove from previous hex
                grid.vacate(currHex);
                // if the sigil merged, don't update grid
                if (sigil.isActive()) {                    
                    // add to new hex
                    grid.occupy(sigil.getHex(), sigil.getEffigy());
                }
            }

            // update the sigil chain
            int maxTries = 30;
            while (maxTries > 0 && result.second > 0) {
                Sigil& chainSigil = sigils.at(result.second);
                HexPoint currChainHex = chainSigil.getHex();

                TraceLog(LOG_INFO, "Start chain move for %d", chainSigil.getEffigy().value);
                result = chainSigil.update(grid, dir);

                // update grid hexes with sigil effigies
                if (currChainHex != chainSigil.getHex()) {
                    grid.vacate(currChainHex);
                    grid.occupy(chainSigil.getHex(), chainSigil.getEffigy());
                }

                // TODO: do chaining sigil ever merge?
                // if (result.first > 0) {
                //     Sigil& mergeSigil = sigils.at(result.first);
                //     mergeSigil.disable();
                //     Effigy mergeEff = mergeSigil.getEffigy();
                //     Effigy chainEff = chainSigil.getEffigy();
                //     TraceLog(LOG_INFO, "Merging %d (%d) and %d (%d)", chainEff.index, chainEff.value, mergeEff.index, mergeEff.value);
                //     chainSigil.setEffigy({ chainEff.index, chainEff.value + mergeEff.value });
                // }
                maxTries--;
            }

        }
    }


    if (!grid.isFull()) {
    // if (sigils.size() < 8) {
        spawnSigil(2);
    }
}

void World::updateGame(){

    grid.update();

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        updateSigils(Direction::UP);
    } else if (IsKeyPressed(KEY_E) || (IsKeyPressed(KEY_UP) && IsKeyPressed(KEY_RIGHT))) {
        updateSigils(Direction::UP_RIGHT);
    } else if (IsKeyPressed(KEY_D) || (IsKeyPressed(KEY_DOWN) && IsKeyPressed(KEY_RIGHT))) {
        updateSigils(Direction::DOWN_RIGHT);
    } else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        updateSigils(Direction::DOWN);
    } else if (IsKeyPressed(KEY_A) || (IsKeyPressed(KEY_DOWN) && IsKeyPressed(KEY_LEFT))) {
        updateSigils(Direction::DOWN_LEFT);
    } else if (IsKeyPressed(KEY_Q) || (IsKeyPressed(KEY_UP) && IsKeyPressed(KEY_LEFT))) {
        updateSigils(Direction::UP_LEFT);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        TraceLog(LOG_INFO, "--------------\n\n\n\n\n\n\n-------------");
    }
    //NOTES: create a vector of Sigil to iterate over
    //Sigils will have a HexPoint that links to Grid
    // For any given direction:
    // Look in the direction in front of the Sigil by getting hexNeighbor
    // if it's empty TODO: add Grid.isEmpty
    // Move Sigil forward, and then look at the neighbor in opposite direciton of movement
    // if there is a sigil behind, repeat / recurse the same algorithm until the edge of the hex grid
    // then repeate the entire algorithm until the leading sigil hits the edge or another sigil
}

void World::spawnSigil(int value) {
    HexPoint spawnPoint = HexPoint(0, 0, 0);

    if (grid.isOccupied(spawnPoint)) {
        spawnPoint = grid.hexFindFirstEmpty();
    }

    placeSigil(spawnPoint, value);
}

void World::placeSigil(HexPoint hex, int value) {
    if (!grid.isValid(hex)) {
        TraceLog(LOG_ERROR, "Bad hex for spawning sigil!");
        return;
    }

    // find disabled sigils if size 
    // is greater than total hexes
    int index = sigils.size();
    if (index >= grid.getTotalHexes()) {
        for (auto& sigil : sigils) {
            if (!sigil.isActive()) {
                sigil.enable();
                Effigy eff = sigil.getEffigy();
                sigil.setEffigy({ eff.index, value });
                grid.occupy(hex, sigil.getEffigy());
                break;
            }
        }
    } else {
        Effigy eff = Effigy(index, value);
        sigils.emplace_back(hex, grid.hexPosition(hex), eff);
        grid.occupy(hex, eff);
    }
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