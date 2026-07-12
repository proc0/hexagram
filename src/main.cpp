#include "app.hpp"

int main(void){
    App* app = new App();
    
    app->load();
    app->start();
    app->unload(0, 0, app);

    delete app;

    return 0;
}

// DONE: add ending game conditions
// DONE: remove return to main menu button

// WORK: test, bug fix, and submit
// WORK: add title screen and remove initial logo
// WORK: add better spawn sigil algo

// TODO: add winning screen
// TODO: update sigil colors
// TODO: add controllers
// TODO: test Web build

// TODO: Add better backgrounds on main menu, title and game screen
// TODO: add how-to-play modal screen on main menu
// TODO: add credits button on main menu
// TODO: add merge effects
// TODO: add spawn effects
// TODO: add basic sounds

