#pragma once

#include "window.hpp"
#include "types.hpp"

#include <raylib.h>

// NOTE: in order to make this more flexible use <numbers> and std::numbers::sqrt2
// consteval float sq3over2() { return std::sqrtf(3.0f) * 0.5f }
constexpr float SQRT3_OVER_2 = 0.86602540378f; 
constexpr float RAD_60 = 60.0f*DEG2RAD;

// TODO: change to floats, review structs and how to integrate with grid
// TODO: implement project, inject, and generate to draw hex grid
// TODO: research how to store/map the grid in array of arrays or maps
struct Orientation {
    const float f0, f1, f2, f3;
    const float b0, b1, b2, b3;
    const float start_angle; // in multiples of 60°
    Orientation(float f0_, float f1_, float f2_, float f3_,
                float b0_, float b1_, float b2_, float b3_,
                float start_angle_)
    : f0(f0_), f1(f1_), f2(f2_), f3(f3_),
      b0(b0_), b1(b1_), b2(b2_), b3(b3_),
      start_angle(start_angle_) {}
};

const Orientation layout_pointy
  = Orientation(sqrtf(3.0f), sqrtf(3.0f) / 2.0f, 0.0f, 3.0f / 2.0f,
                sqrtf(3.0f) / 3.0f, -1.0f / 3.0f, 0.0, 2.0f / 3.0f,
                0.5f);
const Orientation layout_flat
  = Orientation(3.0f / 2.0f, 0.0f, sqrtf(3.0f) / 2.0f, sqrtf(3.0f),
                2.0f / 3.0f, 0.0f, -1.0f / 3.0f, sqrtf(3.0f) / 3.0f,
                0.0f);

struct Layout {
    const Orientation orientation;
    const Vector2 size;
    const Vector2 origin;
    Layout(Orientation orientation_, Vector2 size_, Vector2 origin_)
    : orientation(orientation_), size(size_), origin(origin_) {}
};

typedef struct Hex {
	Vector2 position;
	Vector2 projection;
	bool isClicked;
} Hex;

class Grid : public Layer {
	std::vector<Hex> hexes = {};
    const Window& window;
	Layout layout = Layout(layout_flat, Vector2({ 60.0f, 60.0f }), Vector2({ window.halfWidthf, window.halfHeightf }));


    float radius = 60.0f;

public:
    Grid(const Window& window): window(window) {};
    ~Grid() = default;

    void load();
    
    void generate(int layers);

    void renderGrid() const;
    void drawHex(Vector2 position, bool isClicked) const;

    void updateGrid();
    
    // axial to screen
    Vector2 project(Vector2 axial);
    // screen to axial
    Vector2 inject(Vector2 position);

    void resize(int width, int height) override;
    // void transition(State::Screen);
    // void unload();
};
