#pragma once

#include <raylib.h>

namespace Action {
    enum Surface {
        DO_NOTHING,
        NEW_GAME,
        LOAD_GAME,
        OPTIONS,
        RESUME_GAME,
        MAIN_MENU,
        CONFIRM_RETURN_MAIN,
        CANCEL_RETURN_MAIN,
        QUIT_APP,
    };
}

namespace Event {
    enum Input {
        IDLE,
        PRIMARY,
        PRIMARY_DOWN,
        PRIMARY_UP,
        SECONDARY,
        SECONDARY_DOWN,
        SECONDARY_UP,
        ZOOM_IN,
        ZOOM_OUT,
        SWIPE_UP,
        SWIPE_DOWN,
        SWIPE_RIGHT,
        SWIPE_LEFT,
        KEY_ESCAPE,
        KEY_OTHER
    };

    enum Timer {
        READY,
        RUNNING,
        FINISHED
    };

    enum Surface {
        NO_EVENT,
        SHOW_RETURN_MAIN_MENU_CONFIRMATION,
    };
}

namespace State {
    enum App {
        LOAD,
        RUN,
        PAUSE,
        HALT
    };

    enum Screen {
        INTRO,
        TITLE,
        MAIN,
        GAME
    };

    enum Game {
        START,
        PLAY,
        HOLD,
        OVER,
        END
    };

    enum Pointer {
        IDLE,
        GRAB,
        DRAG,
        DROP
    };
}

// WARNING: assert(length == 6)
// used for Grid.directions
enum Direction {
  UP,
  UP_RIGHT,
  DOWN_RIGHT,
  DOWN,
  DOWN_LEFT,
  UP_LEFT,
};

typedef struct InputEvent {
    Event::Input id;
    Vector2 position;
} InputEvent;

typedef struct GameState {
    int raylibLogoClicks;
    int raylibLogoBounces;
    int raylibLogoCorners;
} GameState;

// typedef struct TimerEvent {
//     uint16_t id;
//     Event::Timer state;
// } TimerEvent;

class Layer {
public:
    virtual ~Layer() = default;
    virtual void resize(int width, int height) = 0;
};

struct HexPoint { 
    // cube coordinates storage 
    int q, r, s;
    // axial coordinates constructor
    // derive the third coordinate s by -q - r
    HexPoint(int q1, int r1): q(q1), r(r1), s(-q1 - r1) {}
    HexPoint(int q1, int r1, int s1): q(q1), r(r1), s(s1) {}
    bool operator==(const HexPoint&) const = default;
};

struct SigilIcon {
    int value;
    int index;
};

struct HexState {
  HexPoint point;
  Vector2 position;
  SigilIcon icon;
  bool isOccupied;
};

struct Matrix2x2Pair {
    const float f0, f1, f2, f3;
    const float b0, b1, b2, b3;
};
