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

// WORK: add title screen and remove initial logo
// WORK: add better spawn sigil algo

// TODO: add controllers
// TODO: test Web build

// TODO: add how-to-play modal screen on main menu
// TODO: add credits button on main menu
// TODO: add merge effects
// TODO: add spawn effects
// TODO: add basic sounds

