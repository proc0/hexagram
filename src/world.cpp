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

    sigils.reserve(grid.getTotalHexes() + 1);

    // ANCHOR sigil
    HexPoint hex = HexPoint(0, 0, 0);
    Effigy eff = Effigy(0, 0);
    sigils.emplace_back(hex, Vector2({}), eff);
    sigils.at(0).disable();

    // createSigil(HexPoint(0, 0, 0), 4);
    // createSigil(HexPoint(0, 2, -2), 4);
    // createSigil(HexPoint(0, 3, -3), 12);
    // createSigil(HexPoint(-2, 0, 2), 2);
    // createSigil(HexPoint(-2, 1, 1), 2);
    // createSigil(HexPoint(3, -2, -1), 2);
    // createSigil(HexPoint(3, 0, -3), 2);
    // createSigil(HexPoint(-2, 3, -1), 8);
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
    // For any given direction:
    // Look in the direction in front of the Sigil by getting hexNeighbor
    // Move Sigil forward, then look at the neighbor in opposite direction of movement.
    // If there is a sigil behind, repeat the same algorithm until the end of the chain,
    // merging only the first sigil in the movement if sigils are the same (i.e. value).
    for (auto& sigil : sigils) {
        if (sigil.isActive()) {
            HexPoint sourceHex = sigil.getHex();
            // update sigil and get a sigil index to merge
            // and a sigil index to start chain move (tail sigils)
            std::pair<int, int> result = sigil.update(grid, dir, false);

            int mergeIndex = result.first;
            int chainIndex = result.second;
            // TODO: move merge process into its own method
            // merge sigils, only head sigil (of chain) merges
            if (mergeIndex > 0) {
                Sigil& mergeSigil = sigils.at(mergeIndex);
                // merge source sigil unto target sigil
                // the underlying sigil "absorbs" this one
                Effigy mergeEffigy = mergeSigil.getEffigy();
                Effigy sourceEffigy = sigil.getEffigy();
                
                TraceLog(LOG_INFO, "Merging %d (%d) and %d (%d)", sourceEffigy.index, sourceEffigy.value, mergeEffigy.index, mergeEffigy.value);

                // create new effigy with the underlying sigil's index and combined values
                Effigy mergedEffigy = { mergeEffigy.index, sourceEffigy.value + mergeEffigy.value };
                mergeSigil.setEffigy(mergedEffigy);

                //TODO: do a vector swap with last item to have all disabled at the end
                // SIGIL + GRID MUTATE
                // tombstone sigil
                sigil.disable();
                // remove from previous hex
                grid.vacate(sourceHex);
                // update grid effigy
                grid.occupy(mergeSigil.getHex(), mergeSigil.getEffigy());
                // flag sigil as merged
                mergeSigil.setMerged(true);

            } else if (sourceHex != sigil.getHex()) {
                // SIGIL + GRID MUTATE
                // if the sigil moved, source hex is
                // different from hex after sigil update
                // remove from previous hex
                grid.vacate(sourceHex);
                // update sigil position
                sigil.setPosition(grid.hexPosition(sigil.getHex()));
                // if the sigil merged, don't update grid
                // add to new hex
                grid.occupy(sigil.getHex(), sigil.getEffigy());
            }

            // safety, max chain < number of hexes x number of sigils?
            // depends on hex grid size, which may vary 
            // TODO: either remove or use something like grid.getSideLength()*chainLength?
            int maxChain = 30;
            while (maxChain > 0 && chainIndex > 0) {
                Sigil& chainSigil = sigils.at(chainIndex);
                HexPoint chainSourceHex = chainSigil.getHex();

                TraceLog(LOG_INFO, "Start chain move for %d (%d)", chainSigil.getEffigy().index, chainSigil.getEffigy().value);
                result = chainSigil.update(grid, dir, true);

                // if the sigil moved, update grid
                HexPoint chainTargetHex = chainSigil.getHex();
                if (chainSourceHex != chainTargetHex) {
                    TraceLog(LOG_INFO, "Updating grid after chain move for %d (%d)", chainSigil.getEffigy().index, chainSigil.getEffigy().value);
                    // SIGIL + GRID MUTATE
                    grid.vacate(chainSourceHex);
                    chainSigil.setPosition(grid.hexPosition(chainTargetHex));
                    grid.occupy(chainTargetHex, chainSigil.getEffigy());
                }
                // update chain index to continue
                chainIndex = result.second;
                maxChain--;
            }
        }
    }

    // clear merged flag on second pass
    for (auto& sigil : sigils) {
        sigil.setMerged(false);
    }

    TraceLog(LOG_INFO, "======= END SIGIL UPDATE =======");

    if (!grid.isFull()) {
        // TODO: spawn at random empty hex location
        TraceLog(LOG_INFO, "Spawning a new sigil");
        
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

}

void World::placeSigil(int index) {

}

void World::spawnSigil(int value) {
    // TODO: better algo for finding empty hexes on grid
    HexPoint spawnPoint = HexPoint(0, 0, 0);

    if (grid.isOccupied(spawnPoint)) {
        spawnPoint = grid.hexFindFirstEmpty();
    }

    createSigil(spawnPoint, value);
}

void World::createSigil(HexPoint hex, int value) {
    if (!grid.isValid(hex)) {
        TraceLog(LOG_ERROR, "Bad hex for spawning sigil!");
        return;
    }

    // find disabled sigils if size 
    // is greater than total hexes
    int sigilsSize = sigils.size();
    if (sigilsSize >= grid.getTotalHexes()) {
        // WARNING: do not use ANCHOR sigil at 0!
        // iterate backwards to get the most likely disabled?
        for (int i = 1; i < sigils.size(); ++i) {
            Sigil& sigil = sigils.at(i);
            if (!sigil.isActive()) {
                sigil.enable();
                // update sigil index to sigils vector
                Effigy effigy = { i, value };
                // SIGIL + GRID MUTATE
                sigil.setEffigy(effigy);
                sigil.setHex(hex);
                sigil.setPosition(grid.hexPosition(hex));
                grid.occupy(hex, effigy);
                sigil.log("Reusing existing sigil.");
                break;
            }
        }
    } else {
        Effigy effigy = Effigy(sigilsSize, value);
        sigils.emplace_back(hex, grid.hexPosition(hex), effigy);
        grid.occupy(hex, effigy);
        sigils.at(sigilsSize).log("Creating new sigil.");
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