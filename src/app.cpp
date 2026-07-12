#include "app.hpp"

#include "config.h"
#include "defaults.hpp"
#include "surface.hpp"
#include "types.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
#include <raylib.h>

void App::load() {
#if DEBUG == 0
    SetTraceLogLevel(LOG_NONE);
#endif

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROJECT_NAME);
    InitAudioDevice();
    
    SetExitKey(KEY_NULL);

	window.load();
    surface.load();
	world.load();
	game.load();

    window.enlist(this);
    window.enlist(&surface);
    window.enlist(&world);
    window.enlist(&game);

    loadTarget();
}

void App::loadTarget() {
    // Render texture to draw, enables window scaling
    // NOTE: If window is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(window.width, window.height);
    targetSource = { 0, 0, static_cast<float>(target.texture.width), -static_cast<float>(target.texture.height) };
    targetDestination = { 0, 0, static_cast<float>(target.texture.width), static_cast<float>(target.texture.height) };
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}

void App::renderLogo() const {
    const char* logoName = "proc0";
    int logoFontSize = 108;
    float logoSize = MeasureText(logoName, logoFontSize);
    int logoX = window.halfWidth - logoSize/2;
    int logoY = window.halfHeight - logoFontSize/2;

    BeginDrawing();
        ClearBackground(RAYWHITE);
        game.renderRaylibLogo();
        DrawText(logoName, logoX, logoY, logoFontSize, BLACK);
    EndDrawing();
}

void App::renderTitle() const {
    const char* gameTitle = "Hex\nMerge";
    int titleFontSize = 112;
    float titleSize = MeasureText(gameTitle, titleFontSize);
    int titleX = window.halfWidth - titleSize/2;
    int titleY = window.halfHeight - titleFontSize*2;

    const char* subtitle = "2048";
    int subtitleFontSize = 80;
    // float subtitleSize = MeasureText(subtitle, subtitleFontSize);
    int subtitleX = window.halfWidth - titleSize/2;
    int subtitleY = titleY+180.0f*window.unit;

    const char* hintText = "Press any key";
    int hintTextFontSize = 32;
    float hintTextSize = MeasureText(hintText, hintTextFontSize);
    int hintTextX = window.halfWidth - hintTextSize/2;
    int hintTextY = static_cast<int>(window.height - window.height*0.25f - hintTextFontSize*0.5f);

    BeginDrawing();
        // ClearBackground(BLANK);
        DrawRectangleGradientV(0, 0, window.width, window.height, bgColor1, bgColor2);

        DrawPoly({ window.halfWidthf-80.0f*window.unit, window.halfHeightf-120.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, BLACK);
        DrawPolyLines({ window.halfWidthf-80.0f*window.unit, window.halfHeightf-120.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, GREEN);
        DrawPolyLines({ window.halfWidthf-60.0f*window.unit, window.halfHeightf-100.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, GREEN);
        DrawPolyLines({ window.halfWidthf-40.0f*window.unit, window.halfHeightf-80.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, GREEN);
        DrawPolyLines({ window.halfWidthf-20.0f*window.unit, window.halfHeightf-60.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, GREEN);
        DrawPolyLines({ window.halfWidthf, window.halfHeightf-40.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, GREEN);
        DrawPolyLines({ window.halfWidthf+20.0f*window.unit, window.halfHeightf-20.0f*window.unit }, 6, 80.0f*window.unit, 0.0f, GREEN);
        DrawPoly({ window.halfWidthf+40.0f*window.unit, window.halfHeightf }, 6, 80.0f*window.unit, 0.0f, BLACK);
        DrawPolyLines({ window.halfWidthf+40.0f*window.unit, window.halfHeightf }, 6, 80.0f*window.unit, 0.0f, GREEN);

        // DrawPoly({ window.halfWidthf, window.halfWidthf }, 6, 200.0f*window.unit, 0.0f, BLANK);
        DrawText(gameTitle, titleX, titleY, titleFontSize, RAYWHITE);
        DrawText(subtitle, subtitleX, subtitleY, subtitleFontSize, RAYWHITE);
        DrawText(hintText, hintTextX, hintTextY, hintTextFontSize, RAYWHITE);
    EndDrawing();
}

void App::intro(void* self) {
    App* app = static_cast<App*>(self);
    app->runIntro();
}

void App::runIntro() {
    // (timer.*timer.update)();

    window.update({ .id = Event::Input::IDLE, .position = Vector2({}) });
    
    // if (screen == State::Screen::INTRO) {
    //     if(input.updateAnyKey() || timer.isEmpty()) {
    //         screen = State::Screen::TITLE;
    //     }

    //     renderLogo();
    // } else 

    if (screen == State::Screen::TITLE) {

        if (titleScreenBGLerp >= 1.0f) {
            titleScreenBGLerp -= 0.001f;
        } else {
            titleScreenBGLerp += 0.001f;
        }
        bgColor1 = ColorLerp(GRAY, BLACK, titleScreenBGLerp);
        bgColor2 = ColorLerp(ORANGE, RED, titleScreenBGLerp);
        
        renderTitle();

        if(input.updateAnyKey()) {
            state = State::App::RUN;
            screen = State::Screen::MAIN;
            surface.transition(state, screen);
            world.transition(screen);

#ifdef __EMSCRIPTEN__
            // cancel the main loop before setting it to run
            emscripten_cancel_main_loop();
            emscripten_set_main_loop_arg(run, this, 0, 0);
#endif
        }
    }

#ifndef __EMSCRIPTEN__
    if (WindowShouldClose()) {
        state = State::App::HALT;
    }
#endif
}

void App::render(Clay_RenderCommandArray& renderCommands) const {
    BeginTextureMode(target);
        ClearBackground(BLANK);
        
        (world.*world.render)();
        (game.*game.render)();
    EndTextureMode();

	BeginDrawing();
        ClearBackground(GRAY);
        
        DrawTexturePro(target.texture, targetSource, targetDestination, Vector2({}), 0.0f, WHITE);

        (surface.*surface.render)(renderCommands);

        // DrawFPS(50, 50);
	EndDrawing();
}

void App::run(void* self) {
    App* app = static_cast<App*>(self);

    Clay_RenderCommandArray renderCommands = app->update();
    app->render(renderCommands);
}

void App::start() {
    // set intro timer
    timer.schedule(INTRO_TIME_MS, nullptr);
	
#ifdef __EMSCRIPTEN__
    // no target FPS (3rd param) to allow browser to optimize frame rate
    // set simulate infinite loop (4th param) to 0 to let the rest of the function execute (on Web)
    emscripten_set_main_loop_arg(intro, this, 0, 1);
    // set the main loop directly to run here if any performance issues on Web
    // and set the App state to Run before running
    // emscripten_set_main_loop_arg(run, this, 0, 0);
    emscripten_set_beforeunload_callback(this, unload);
#else
    SetTargetFPS(TARGET_FPS);

    while (state != State::App::RUN && state != State::App::HALT) {
        intro(this);
    }

    while (state != State::App::HALT) {
        run(this);
    }
#endif
}

Clay_RenderCommandArray App::update() {

#ifndef __EMSCRIPTEN__
    if (WindowShouldClose()) {
        state = State::App::HALT;
        return Clay_RenderCommandArray({ 0, 0, nullptr });
    }
#endif

    (timer.*timer.update)();
    InputEvent inputEvent = input.update();
    window.update(inputEvent);

    Action::Surface surfaceAction = (surface.*surface.update)(inputEvent);

    if(screen == State::Screen::GAME) {
        if(inputEvent.id == Event::Input::KEY_ESCAPE){
            if(state == State::App::PAUSE) {
                TraceLog(LOG_INFO, "UNPAUSE");
                state = State::App::RUN;

                surface.transition(state, screen);

            } else if (state == State::App::RUN) {
                TraceLog(LOG_INFO, "PAUSE");
                state = State::App::PAUSE;

                surface.transition(state, screen);
            }
        } else if (state == State::App::PAUSE || game.isOver()) {

            if (surfaceAction == Action::Surface::RESUME_GAME) {
                TraceLog(LOG_INFO, "UNPAUSE");
                state = State::App::RUN;
            
                surface.transition(state, screen);          
            
            } else if (surfaceAction == Action::Surface::RESTART_GAME) {
                TraceLog(LOG_INFO, "RESTART");
                state = State::App::RUN;
                
                world.restart();
                game.restart();         
                surface.transition(state, screen);          
            
            } else if (surfaceAction == Action::Surface::MAIN_MENU) {
            
                surface.beginEvent(Event::Surface::SHOW_RETURN_MAIN_MENU_CONFIRMATION);
            
            } else if (surfaceAction == Action::Surface::CONFIRM_RETURN_MAIN) {
                surface.clearEvent();
                state = State::App::RUN;
                screen = State::Screen::MAIN;

                world.transition(screen);
                game.transition(screen);
                surface.transition(state, screen);

            } else if (surfaceAction == Action::Surface::CANCEL_RETURN_MAIN) {

                surface.clearEvent();
            
            } else if (surfaceAction == Action::Surface::QUIT_APP) {
                state = State::App::HALT;
                return Clay_RenderCommandArray({ 0, 0, nullptr });
            }
        }
    } else if (screen == State::Screen::MAIN) {
        if(surfaceAction == Action::Surface::NEW_GAME) {
            state = State::App::RUN;
            screen = State::Screen::GAME;
            
            world.transition(screen);
            game.transition(screen);
            surface.transition(state, screen);

        } else if (surfaceAction == Action::Surface::QUIT_APP) {
            state = State::App::HALT;
            return Clay_RenderCommandArray({ 0, 0, nullptr });
        }            
    }

    Clay_BeginLayout();
	GameState gameState = (game.*game.update)(state, surfaceAction, inputEvent);
	(world.*world.update)(surfaceAction);
    (surface.*surface.display)(gameState);
    (surface.*surface.menu)();
    Clay_RenderCommandArray renderCommands = Clay_EndLayout(GetFrameTime());

    return renderCommands;
}

void App::resize(int width, int height) {
    if (screen == State::Screen::INTRO) {
        game.loadRaylibLogo();
    }
    
    UnloadRenderTexture(target);
    loadTarget();
}

const char* App::unload(int eventType, const void *reserved, void *self) {
    App* app = static_cast<App*>(self);
    
	app->game.unload();
	app->world.unload();
    app->surface.unload();

    UnloadRenderTexture(app->target);

    CloseAudioDevice();
    CloseWindow();

    return nullptr;
}

