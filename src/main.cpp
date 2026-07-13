#include "app.hpp"

int main(void){
    App* app = new App();
    
    app->load();
    app->start();
    app->unload(0, 0, app);

    delete app;

    return 0;
}

// TODO: remove indices from Effigy, put them on Sigil
// TODO: redo the main algo:
// NOTE: it should be done in multiple passes
// NOTE: first move sigils that can move, dragging their chains
// NOTE: then merge all sigils that can merge, dragging their chains
// NOTE: Figure out what to do when multiple merges happen in the same chain


// NOTE: instead of iterating through sigils, iterate through grid and pull sigils up
// iterate starting from the edge direction of movement (if up arrow pressed
// then start at the top left edge to top right edge, and move downwards)
// move and merge each sigil encountered and flag them as merged so the 
// subsequent sigils cannot merge
// Summary: walk grid in opposite of movement direction, move + merge + flag as merged for each
// sigil, and don't merge is if it's flag on that move.
// NOTE: how to iterate efficiently through hex grid from multiple directions? 
// so a sorted data structure of some kind that can be traversed depending on what direction was pressed

// NOTE: finding a programmatic way to find and traverse edges from grid:
// edges will have at least one of their q,r,s coordinates = # of "onion layers" (not counting center)
// so a hex grid with 7 hexes in the center (has two onion layers on center) has edges with at least one = abs(2)
// invariant
// top edge: s == 2 || r == -2
// top right edge: r == -2 || q == 2

// TODO: fix grid fitting to screen properly on resize, orientation, or aspect ratio
// TODO: fix controllers not resizing properly on screen change
// TODO: fix place where controllers are 
// TODO: fix numbers in sigils not resizing properly
// TODO: update styles, no gradients, better colors
// TODO: update effects
// TODO: add sounds
// TODO: fix score not resetting and getting mangled on game over screen
// TODO: rebalance game
// TODO: do not spawn sigils unless one sigil has moved
// TODO: implement frame-independent animations and effects
// TODO: add combo effects
// TODO: memoize grid render, only render once to target, reuse that target

// TODO: experiment with time based approach, sigils appear every few seconds and speed up as it gets harder?
// NOTE: wipeout meets 2048?
// TODO: experiment with starting with a small grid, then making grid bigger
// NOTE: experiment with sigil movement speeding up as time go by, and also sigil spawning speeding up
// this would be like Wipeout "pure races" where things speed up until you fail
// this would be survival mode 2048 vs. turn based
// theme can be starts slow with glaciers and ice, and speeds up until there is fire and everything moves quickly
