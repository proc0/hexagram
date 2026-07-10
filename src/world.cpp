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
    TraceLog(LOG_INFO, "======= BEGIN SIGIL UPDATE =======");
    for (auto& sigil : sigils) {
        if (sigil.isActive()) {
            HexPoint sourceHex = sigil.getHex();
            // update sigil and get a sigil index to merge
            // and a sigil index to start chain move (tail sigils)
            std::pair<int, int> result = sigil.update(grid, dir);

            int mergeIndex = result.first;
            int chainIndex = result.second;

            // TODO: move merge process into its own method
            // merge sigils, only head sigil (of chain) merges
            if (mergeIndex > 0) {
                Sigil& mergeSigil = sigils.at(mergeIndex);
                // mergeSigil.disable();
                
                // merge source sigil unto target sigil
                // the underlying sigil "absorbs" this one
                Effigy mergeEffigy = mergeSigil.getEffigy();
                Effigy sourceEffigy = sigil.getEffigy();
                
                TraceLog(LOG_INFO, "Merging %d (%d) and %d (%d)", sourceEffigy.index, sourceEffigy.value, mergeEffigy.index, mergeEffigy.value);
                
                // TODO: remove this dead code if strategy works
                // this is when current sigil "lands on" underlying sigil and replaces it
                // Effigy newEff = { sourceEffigy.index, sourceEffigy.value + mergeEffigy.value };
                
                // tombstone sigil
                sigil.disable();
                // create new effigy with the underlying sigil's index and combined values
                Effigy newEff = { mergeEffigy.index, sourceEffigy.value + mergeEffigy.value };
                mergeSigil.setEffigy(newEff);
                // update grid effigy
                grid.occupy(mergeSigil.getHex(), mergeSigil.getEffigy());
            }

            // if the sigil moved, update grid
            if (sourceHex != sigil.getHex()) {
                // remove from previous hex
                grid.vacate(sourceHex);
                // if the sigil merged, don't update grid
                if (sigil.isActive()) {                    
                    // add to new hex
                    grid.occupy(sigil.getHex(), sigil.getEffigy());
                }
            }

            // safety, max chain < number of hexes x number of sigils?
            // depends on hex grid size, which may vary 
            // TODO: either remove or use something like grid.getSideLength()*chainLength?
            int maxChain = 30;
            while (maxChain > 0 && chainIndex > 0) {
                Sigil& chainSigil = sigils.at(chainIndex);
                HexPoint chainHex = chainSigil.getHex();

                TraceLog(LOG_INFO, "Start chain move for %d (%d)", chainSigil.getEffigy().index, chainSigil.getEffigy().value);
                result = chainSigil.update(grid, dir);

                // if the sigil moved, update grid
                if (chainHex != chainSigil.getHex()) {
                    TraceLog(LOG_INFO, "Updating grid after chain move for %d (%d)", chainSigil.getEffigy().index, chainSigil.getEffigy().value);
                    grid.vacate(chainHex);
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
                maxChain--;
            }

        }
    }

    TraceLog(LOG_INFO, "======= END SIGIL UPDATE =======");

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
        // WARNING: do not use SENTINEL sigil at index 0!
        for (int i = 1; i < sigils.size(); ++i) {
            Sigil& sigil = sigils.at(i);
            if (!sigil.isActive()) {
                sigil.enable();
                // update sigil index to sigils vector
                sigil.setEffigy({ sigil.getEffigy().index, value });
                // make sure to get effigy again after update
                grid.occupy(hex, sigil.getEffigy());
                sigil.log("Re-enabling sigil for reuse.");
                break;
            }
        }
    } else {
        Effigy eff = Effigy(index, value);
        sigils.emplace_back(hex, grid.hexPosition(hex), eff);
        grid.occupy(hex, eff);
        sigils.at(index).log("Spawning new sigil.");
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