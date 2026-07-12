#include "world.hpp"

#include "raylib.h"
#include "types.hpp"

void World::load(){
    // std::string pathAssets = DIR_ASSETS;
    // const char* pathSoundSplat = pathAssets.append("/").append(URI_SOUND_SPLAT).c_str();

    // splat = LoadSound(pathSoundSplat);

    grid.load();

    sigils.reserve(grid.getTotalHexes() + 1);

    // ANCHOR sigil
    HexPoint hex = HexPoint(0, 0, 0);
    Effigy eff = Effigy(0, 0);
    sigils.emplace_back(hex, eff, Vector2({}));
    sigils.at(0).disable();

    // fill chance array with initial value
    // sigilHat.fill(2);

    createSigil(HexPoint(0, 0, 0), 2);
    // createSigil(HexPoint(0, 2, -2), 4);
    // createSigil(HexPoint(0, 3, -3), 12);
    // createSigil(HexPoint(-2, 0, 2), 2);
    // createSigil(HexPoint(-2, 1, 1), 2);
    // createSigil(HexPoint(3, -2, -1), 2);
    // createSigil(HexPoint(3, 0, -3), 2);
    // createSigil(HexPoint(-2, 3, -1), 8);
}

void World::restart() {
    for (auto& sigil : sigils) {
        sigil.disable();
    }
    grid.clear();
    createSigil(HexPoint(0, 0, 0), 2);
}

void World::renderUnit() const {

}

void World::renderMain() const {
    DrawRectangleGradientH(0, 0, window.width, window.height, GREEN, PINK);
}

void World::renderGame() const {
    DrawRectangleGradientV(0, 0, window.width, window.height, BLUE, GREEN);
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

    // TraceLog(LOG_INFO, "======= BEGIN SIGIL UPDATE =======");
    // For any given direction:
    // Look in the direction in front of the Sigil by getting hexNeighbor
    // Move Sigil forward, then look at the neighbor in opposite direction of movement.
    // If there is a sigil behind, repeat the same algorithm until the end of the chain,
    // merging only the first sigil in the movement if sigils are the same (i.e. value).
    int maxValueMerged = 0;
    for (auto& sigil : sigils) {
        // if (sigil.isActive() && !sigil.hasBeenAbsorbed() && !sigil.hasMerged()) {
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
                // TODO: check if this conditional is needed
                // if (!mergeSigil.hasMerged() && !mergeSigil.hasBeenAbsorbed()) {               
                    // merge source sigil unto target sigil
                    // the underlying sigil "absorbs" this one
                    Effigy mergeEffigy = mergeSigil.getEffigy();
                    Effigy sourceEffigy = sigil.getEffigy();
                    
                    // TraceLog(LOG_INFO, "Merging %d (%d) and %d (%d)", sourceEffigy.index, sourceEffigy.value, mergeEffigy.index, mergeEffigy.value);

                    // create new effigy with the underlying sigil's index and combined values
                    int mergedValue = sourceEffigy.value + mergeEffigy.value;
                    Effigy mergedEffigy = { mergeEffigy.index, mergedValue };
                    maxValueMerged = mergedValue > maxValueMerged ? mergedValue : maxValueMerged;
                    // flag sigil as merged
                    mergeSigil.setMerged(true);
                    mergeSigil.setEffigy(mergedEffigy);

                    //TODO: do a vector swap with last item to have all disabled at the end
                    // SIGIL + GRID MUTATE
                    // tombstone sigil
                    // sigil.disable();
                    // update sigil position
                    sigil.setPosition(grid.hexPosition(mergeSigil.getHex()));
                    sigil.beginMovement();
                    sigil.setAbsorbed(true);
                    // remove from previous hex
                    grid.vacate(sourceHex);
                    // update grid effigy
                    grid.occupy(mergeSigil.getHex(), mergeSigil.getEffigy());
                // }

            } else if (sourceHex != sigil.getHex()) {
                // SIGIL + GRID MUTATE
                // if the sigil moved, source hex is
                // different from hex after sigil update
                // remove from previous hex
                grid.vacate(sourceHex);
                // update sigil position
                sigil.setPosition(grid.hexPosition(sigil.getHex()));
                sigil.beginMovement();
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

                // TraceLog(LOG_INFO, "Start chain move for %d (%d)", chainSigil.getEffigy().index, chainSigil.getEffigy().value);
                result = chainSigil.update(grid, dir, true);

                // if the sigil moved, update grid
                HexPoint chainTargetHex = chainSigil.getHex();
                if (chainSourceHex != chainTargetHex) {
                    // TraceLog(LOG_INFO, "Updating grid after chain move for %d (%d)", chainSigil.getEffigy().index, chainSigil.getEffigy().value);
                    // SIGIL + GRID MUTATE
                    grid.vacate(chainSourceHex);
                    chainSigil.setPosition(grid.hexPosition(chainTargetHex));
                    chainSigil.beginMovement();
                    grid.occupy(chainTargetHex, chainSigil.getEffigy());
                }
                // update chain index to continue
                chainIndex = result.second;
                maxChain--;
            }
        }
    }

    // TraceLog(LOG_INFO, "======= END SIGIL UPDATE =======");

    if (isMaxSigilValue(maxValueMerged)) {
        maxSigilValue = maxValueMerged;
        // if (maxSigilValue < 16) {
        //     // only spawn sigils below 32
        //     // int maxSigils = sigilChance.size();
        //     // TODO: weigh each sigil by its value (2 is more freq, 64 less)
        //     sigilChance[maxSigilValue] = 1.0f/maxSigilValue;

        //     int cursor = 0;
        //     for (auto& [key, val] : sigilChance) {
        //         int chance = static_cast<int>(floor(val*sigilHat.size()));
        //         for (int i = cursor; i < chance && i < sigilHat.size(); ++i) {
        //             sigilHat[i] = key;
        //         }
        //         cursor = chance;
        //     }
        // }
    }

    state = State::World::ANIMATING;
}

void World::updateGame(){

    grid.update();

    if (state == State::World::ANIMATING) {
        bool stillMoving = false;
        for (auto& sigil : sigils) {
            sigil.updateMovement();
            if (sigil.isMoving()) {
                stillMoving = true;
            }
        }

        if (!stillMoving) {

            // clear merged flag on second pass
            for (auto& sigil : sigils) {
                if (sigil.hasMerged()) {
                    sigil.finishMerge();
                    sigil.setMerged(false);
                }

                if (sigil.hasBeenAbsorbed()) {
                    sigil.disable();
                    sigil.setAbsorbed(false);
                }
            }

            // TODO: when grid is full check all possible merges before ending game
            if (!grid.isFull()) {
                // TODO: spawn at random empty hex location
                // TraceLog(LOG_INFO, "Spawning a new sigil");
                
                int nextValue = 2;
                int sigProb = 0;
                switch(maxSigilValue) {
                    case 16:
                        sigProb = GetRandomValue(0, 1);
                        nextValue = sigProb ? 2 : 4;
                        break;
                    case 128:
                        sigProb = GetRandomValue(0, 4);
                        if (sigProb == 2) {
                            nextValue = 4;
                        } else if (sigProb == 3) {
                            nextValue = 8;
                        } else {
                            nextValue = 2;
                        }
                        break;
                    default:
                        nextValue = 2;
                }

                spawnSigil(nextValue);
            }

            state = State::World::WAITING;
        }
    }

    if (state == State::World::WAITING) {

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            updateSigils(Direction::UP);
        } else if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_RIGHT)) {
            updateSigils(Direction::UP_RIGHT);
        } else if (IsKeyPressed(KEY_D)) {
            updateSigils(Direction::DOWN_RIGHT);
        } else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            updateSigils(Direction::DOWN);
        } else if (IsKeyPressed(KEY_A)) {
            updateSigils(Direction::DOWN_LEFT);
        } else if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_LEFT)) {
            updateSigils(Direction::UP_LEFT);
        }

        // if (IsKeyPressed(KEY_SPACE)) {
        //     TraceLog(LOG_INFO, "--------------\n\n\n\n\n\n\n-------------");
        //     std::string frames = "";
        //     for (auto& frame : ANIM_FRAMES) {
        //         frames = std::format("{} {}", frames, frame);
        //     }

        //     const char* frameText = TextFormat(frames.c_str());
        //     TraceLog(LOG_INFO, frameText);
        // }
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
                // sigil.setEffigy(effigy);
                // sigil.setHex(hex);
                // sigil.setPosition(grid.hexPosition(hex));
                sigil.reset(hex, effigy, grid.hexPosition(hex));
                grid.occupy(hex, effigy);
                // sigil.log("Reusing existing sigil.");
                break;
            }
        }
    } else {
        Effigy effigy = Effigy(sigilsSize, value);
        sigils.emplace_back(hex, effigy, grid.hexPosition(hex));
        grid.occupy(hex, effigy);
        // sigils.at(sigilsSize).log("Creating new sigil.");
    }
}

bool World::isMoveAvailable() const {
    for (auto& sigil : sigils) {
        if (sigil.canMove(grid)) return true;
    }

    return false;
}

bool World::isGridLocked() const {
    return grid.isFull() && !isMoveAvailable();
}

bool World::isGridFull() const {
    return grid.isFull();
}

bool World::isMaxSigilValue(int value) const {
    for (auto& sigil : sigils) {
        if (value > sigil.getEffigy().value) {
            return true;
        }
    }

    return false;
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