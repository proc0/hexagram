#pragma once

#include <raylib.h>
#include <array>

namespace Action {
    enum Surface {
        DO_NOTHING,
        HUD_UP,
        HUD_UP_RIGHT,
        HUD_DOWN_RIGHT,
        HUD_DOWN,
        HUD_DOWN_LEFT,
        HUD_UP_LEFT,
        NEW_GAME,
        RESTART_GAME,
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
        WIN,
        OVER
    };

    enum Pointer {
        IDLE,
        GRAB,
        DRAG,
        DROP
    };

    enum Sigil {
        STILL,
        MOVING,
        EFFECT
    };

    enum World {
        WAITING,
        ANIMATING,
        GRIDLOCK
    };
}

namespace Kind {
    enum SigilEffect {
        NORMAL,
        MERGE,
        SPAWN
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
  UP_LEFT
};

static constexpr std::array<Direction, 6> ALL_DIRECTIONS = { 
    Direction::UP,
    Direction::UP_RIGHT,
    Direction::DOWN_RIGHT,
    Direction::DOWN,
    Direction::DOWN_LEFT,
    Direction::UP_LEFT
};

static inline Direction oppositeDir(Direction dir) {
    Direction result = dir;

    switch (dir) {
        case Direction::UP:
            result = Direction::DOWN;
            break;
        case Direction::UP_RIGHT:
            result = Direction::DOWN_LEFT;
            break;
        case Direction::DOWN_RIGHT:
            result = Direction::UP_LEFT;
            break;
        case Direction::DOWN:
            result = Direction::UP;
            break;
        case Direction::DOWN_LEFT:
            result = Direction::UP_RIGHT;
            break;
        case Direction::UP_LEFT:
            result = Direction::DOWN_RIGHT;
            break;
        default:
            result = dir;
    }

    return result;
}

typedef struct InputEvent {
    Event::Input id;
    Vector2 position;
} InputEvent;

typedef struct GameState {
    State::Game state;
    int score;
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
    constexpr HexPoint(int q1, int r1): q(q1), r(r1), s(-q1 - r1) {}
    constexpr HexPoint(int q1, int r1, int s1): q(q1), r(r1), s(s1) {}
    bool operator==(const HexPoint&) const = default;
};

struct Effigy {
    int index;
    int value;
};

struct HexState {
  HexPoint point;
  Vector2 position;
  Effigy effigy;
  bool isOccupied;
};

struct Matrix2x2Pair {
    const float f0, f1, f2, f3;
    const float b0, b1, b2, b3;
};

// NOTE: to rotate, swap two components in this pattern:
// swap first and last (outer)
// swap second and last (tail)
// swap first and second (head)
// wherever the 0 is determines which swap
// if 0 is first, -> outer swap
// if 0 is second -> head swap
// if 0 is third -> tail swap

// clockwise swap is: outer -> tail -> head (or head -> outer -> tail )
// counter clockwise swap is: tail -> outer -> head (or outer -> head -> tail )

// i.e. first determine where zero is
// Start NORTH -> (0,-1,1) go clockwise:
// 0 is first, for clockwise, this is an outer swap
// swap q and s (0,-1,1) -> (1,-1,0) for outer swap

// clockwise swap map:
// q = 0 -> outer swap 
// r = 0 -> head swap
// s = 0 -> tail swap

// counter-clockwise swap map:
// q = 0 -> head swap
// r = 0 -> tail swap
// s = 0 -> outer swap

// the zero is what wraps around
// the zero moves "forward" or "backward" in the components
// depending on how you rotate
// rotate clockwise, zero moves from q -> r -> s
// rotate counter clockwise zero moves from s -> r -> q

struct HexDirection {
    static constexpr HexPoint NORTH      {  0, -1,  1 };
    static constexpr HexPoint NORTH_EAST {  1, -1,  0 };
    static constexpr HexPoint SOUTH_EAST {  1,  0, -1 };
    static constexpr HexPoint SOUTH      {  0,  1, -1 };
    static constexpr HexPoint SOUTH_WEST { -1,  1,  0 };
    static constexpr HexPoint NORTH_WEST { -1,  0,  1 };
};

static inline HexPoint oppositeHexDir(HexPoint dir) {
    HexPoint result = dir;

        if (dir == HexDirection::NORTH) {

            result = HexDirection::SOUTH;
        } else if (dir == HexDirection::NORTH_EAST) {

            result = HexDirection::SOUTH_WEST;
        } else if (dir == HexDirection::SOUTH_EAST) {

            result = HexDirection::NORTH_WEST;
        } else if (dir == HexDirection::SOUTH) {

            result = HexDirection::NORTH;
        } else if (dir == HexDirection::SOUTH_WEST) {

            result = HexDirection::NORTH_EAST;
        } else if (dir == HexDirection::NORTH_WEST) {

            result = HexDirection::SOUTH_EAST;
        }


    return result;
}