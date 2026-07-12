#include "world.hpp"

#include "raylib.h"
#include "types.hpp"
// #include <cmath>

void World::load(){
    // std::string pathAssets = DIR_ASSETS;
    // const char* pathSoundSplat = pathAssets.append("/").append(URI_SOUND_SPLAT).c_str();

    // splat = LoadSound(pathSoundSplat);

    grid.load();
    window.enlist(&grid);
    grid.resize(window.width, window.height);

    sigils.reserve(grid.getTotalHexes() + 1);

    // ANCHOR sigil
    HexPoint hex = HexPoint(0, 0, 0);
    Effigy eff = Effigy(0, 0);
    sigils.emplace_back(hex, eff, Vector2({}));
    sigils.at(0).disable();

    // fill chance array with initial value
    // sigilHat.fill(2);

    createSigil(HexPoint(0, 0, 0), 2);
    sigils.at(1).resize(window.unit);
    // createSigil(HexPoint(0, 2, -2), 4);
    // createSigil(HexPoint(0, 3, -3), 12);
    // createSigil(HexPoint(-2, 0, 2), 2);
    // createSigil(HexPoint(-2, 1, 1), 2);
    // createSigil(HexPoint(3, -2, -1), 2);
    // createSigil(HexPoint(3, 0, -3), 2);
    // createSigil(HexPoint(-2, 3, -1), 8);
}

void World::restart() {
    state = State::World::WAITING;

    for (auto& sigil : sigils) {
        sigil.disable();
    }
    grid.clear();
    grid.reset();

    maxSigilValue = 2;
    phaseChange();
    createSigil(HexPoint(0, 0, 0), 2);
}

void World::renderUnit() const {

}

void World::renderMain() const {
    DrawRectangleGradientV(0, 0, window.width, window.height, phaseColor1, phaseColor2);
    grid.render();
}

void World::renderGame() const {
    DrawRectangleGradientV(0, 0, window.width, window.height, phaseColor1, phaseColor2);
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
                    score += mergedValue;
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

    if (isMaxSigilValue(maxValueMerged) && maxSigilValue != maxValueMerged) {
        maxSigilValue = maxValueMerged;
        TraceLog(LOG_INFO, "MAX SIGIL IS: %d", maxSigilValue);
        phaseChangeEvent = true;
    }

    state = State::World::ANIMATING;
}

void World::updateGame(){
    if (state == State::World::GRIDLOCK) return;

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

            if (grid.isFull()) {
                TraceLog(LOG_INFO, "GRID IS FULL");
                if (!isMoveAvailable()) {
                    TraceLog(LOG_INFO, "GRID IS LOCKED");
                    state = State::World::GRIDLOCK;
                    return;
                }
            } else {

                if (maxSigilValue > 1024) {
                    spawnSigil(getRandomSigilValue());
                    spawnSigil(getRandomSigilValue());
                    spawnSigil(getRandomSigilValue());
                } else if (maxSigilValue > 512) {
                    spawnSigil(getRandomSigilValue());
                    spawnSigil(getRandomSigilValue());
                } else {
                    spawnSigil(getRandomSigilValue());
                }

                if (phaseChangeEvent) {
                    phaseChange();
                    grid.phaseChange(maxSigilValue);
                    phaseChangeEvent = false;
                }
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

    // if (phaseColorLerp >= 1.0f && !phaseColorLerpSwitch) {
    //     phaseColorLerp -= 0.01f;
    //     phaseColorLerpSwitch = phaseColorLerp < 0.5f;
    // } else {
    //     phaseColorLerp += 0.01f;
    //     phaseColorLerpSwitch = phaseColorLerp < 1.0f;
    // }

    // phaseColorLerp += 0.01f;
    // if (phaseColorLerp >= 1.0f) {
    //     phaseColorLerp = 0.5f;
    // }
    // // TraceLog(LOG_INFO, "%f", sinf(phaseColorLerp));
    // phaseColor2 = ColorLerp(phaseColor2, phaseColor1, 1.0f-phaseColorLerp);

}

void World::placeSigil(int index) {

}

void World::spawnSigil(int value) {
    // TODO: better algo for finding empty hexes on grid
    // HexPoint spawnPoint = grid.hexFindFirstEmpty();

    // if (grid.isOccupied(spawnPoint)) {
    //     spawnPoint = grid.hexFindFirstEmpty();
    // }

    createSigil(grid.hexFindRandomEmpty(), value);
}

void World::createSigil(HexPoint hex, int value) {
    if (!grid.isValid(hex)) {
        TraceLog(LOG_ERROR, "Bad hex for spawning sigil!");
        return;
    }

    // find disabled sigils if size 
    // is greater than total hexes
    int sigilsSize = sigils.size();
    if (sigilsSize > grid.getTotalHexes()) {
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
        // TODO: figure out how to remove need to pass in unit
        // and call resize every time. Should Sigil be a Layer and 
        // have its own resize?
        sigils.back().resize(window.unit);
        grid.occupy(hex, effigy);
        // sigils.at(sigilsSize).log("Creating new sigil.");
    }
}

void World::phaseChange() {
    switch(maxSigilValue) {
        case 2:
            phaseColor1 = ColorLerp(BLUE, WHITE, 0.5f);
            phaseColor2 = ColorLerp(GREEN, WHITE, 0.5f);
            break;
        case 32:
            phaseColor1 = ColorLerp(BLUE, WHITE, 0.5f);
            phaseColor2 = BLUE;
            break;
        case 128:
            phaseColor1 = DARKBLUE;
            phaseColor2 = GOLD;
            break;
        case 512:
            phaseColor1 = BLACK;
            phaseColor2 = MAGENTA;
            break;
        case 1024:
            phaseColor1 = BLACK;
            phaseColor2 = RED;
            break;
        default:
            return;
    }
}

int World::getRandomSigilValue() const {
    int nextValue = 2;
    int sigProb = 0;
    switch(maxSigilValue) {
        case 32:
            sigProb = GetRandomValue(0, 4);
            if (sigProb == 4) {
                nextValue = 4;
            } else {
                nextValue = 2;
            }
            break;
        case 128:
            sigProb = GetRandomValue(0, 6);
            if (sigProb == 6) {
                nextValue = 8;
            } else if (sigProb == 3 || sigProb == 4 || sigProb == 5) {
                nextValue = 4;
            } else {
                nextValue = 2;
            }
            break;
        case 512:
            sigProb = GetRandomValue(0, 6);
            if (sigProb == 6) {
                nextValue = 16;
            } else if (sigProb == 5 || sigProb == 4) {
                nextValue = 8;
            } else if (sigProb == 3 || sigProb == 2) {
                nextValue = 4;
            } else {
                nextValue = 2;
            }
            break;
        default:
            nextValue = 2;
    }

    return nextValue;
}

int World::getMaxSigilValue() const {
    return maxSigilValue;
}

int World::getScore() const {
    return score;
}

bool World::isMoveAvailable() const {
    for (auto& sigil : sigils) {
        if (sigil.canMove(grid)) return true;
    }

    return false;
}

bool World::isGridLocked() const {
    return state == State::World::GRIDLOCK;
}

bool World::isGridFull() const {
    return grid.isFull();
}

bool World::isMaxSigilValue(int value) const {
    bool isMax = true;
    for (auto& sigil : sigils) {
        if (value < sigil.getEffigy().value) {
            isMax = false;
            break;
        }
    }

    return isMax;
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
    for (auto& sigil : sigils) {
        sigil.resize(window.unit);
        sigil.setPosition(grid.project(sigil.getHex()));
    }
}