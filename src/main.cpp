#include "app.hpp"

int main(void){
    App* app = new App();
    
    app->load();
    app->start();
    app->unload(0, 0, app);

    delete app;

    return 0;
}

// TODO: add ending game conditions
// TODO: add better spawn sigil algo
// TODO: add title screen and remove initial logo
// TODO: remove return to main menu button
// TODO: add winning screen
// TODO: test Web build
// TODO: add credits button on main menu
// TODO: Add better backgrounds on main menu, title and game screen
// TODO: update sigil colors
// TODO: add merge effects
// TODO: add spawn effects
// TODO: test, bug fix, and submit
