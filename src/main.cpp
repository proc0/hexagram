#include "app.hpp"

int main(void){
    App* app = new App();
    
    app->load();
    app->start();
    app->unload(0, 0, app);

    delete app;

    return 0;
}

// GOAL: test, bug fix, and submit

// DONE: add ending game conditions
// DONE: remove return to main menu button
// DONE: add winning screen
// DONE: Add better backgrounds on main menu, title and game screen
// DONE: update sigil colors
// DONE: add title screen and remove initial logo
// DONE: test Web build
// DONE: add better spawn sigil algo
// DONE: add controllers
// DONE: add merge effects
// DONE: add spawn effects

// TODO: add basic sounds
// TODO: add credits button on main menu

