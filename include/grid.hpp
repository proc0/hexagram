#pragma once

#include "window.hpp"
#include "types.hpp"

#include <raylib.h>

#include <unordered_map>
#include <array>

#define HEX_SIZE 60.0f

struct HexPoint { 
    // cube coordinates storage 
    int q, r, s;
    // axial coordinates constructor
    // derive the third coordinate s by -q - r
    HexPoint(int q1, int r1): q(q1), r(r1), s(-q1 - r1) {}
    HexPoint(int q1, int r1, int s1): q(q1), r(r1), s(s1) {}
    bool operator==(const HexPoint&) const = default;
};

struct HexState {
  HexPoint point;
  Vector2 position;
  bool isActive = false;
};

struct Matrix2x2Pair {
    const float f0, f1, f2, f3;
    const float b0, b1, b2, b3;
};

// https://www.redblobgames.com/grids/hexagons/implementation.html
namespace std {
    template <> struct hash<HexPoint> {
        size_t operator()(const HexPoint& h) const {
            hash<int> int_hash;
            size_t hq = int_hash(h.q);
            size_t hr = int_hash(h.r);
            return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
        }
    };
}

enum HexDirection {
  UP,
  UP_RIGHT,
  DOWN_RIGHT,
  DOWN,
  DOWN_LEFT,
  UP_LEFT,
};

class Grid : public Layer {
  std::unordered_map<HexPoint, HexState> map;

  // flat top
  const Matrix2x2Pair view = Matrix2x2Pair({
    3.0f/2.0f, 0.0f, sqrtf(3.0f)/2.0f, sqrtf(3.0f),
    2.0f/3.0f, 0.0f, -1.0f/3.0f, sqrtf(3.0f)/3.0f
  });

  const std::array<HexPoint, 6> directions = {
    HexPoint(0, -1, 1), HexPoint(1, -1, 0), HexPoint(1, 0, -1),   
    HexPoint(0, 1, -1), HexPoint(-1, 1, 0), HexPoint(-1, 0, 1)
  };
  // pointy top
  // const Matrix2x2Pair view = Matrix2x2Pair({
  //   sqrtf(3.0f), sqrtf(3.0f)/2.0f, 0.0f, 3.0f/2.0f,
  //   sqrtf(3.0f)/3.0f, -1.0f/3.0f, 0.0, 2.0f/3.0f
  // });
  HexPoint activeHex = HexPoint(0, 0, 0);

  const Window& window;

  Vector2 origin = { window.halfWidthf, window.halfHeightf };
  float hexSize = HEX_SIZE;

public:
    Grid(const Window& window): window(window) {};
    ~Grid() = default;

    void load();
    void generate(int layers);

    void renderGrid() const;
    void renderHex(const HexState&) const;

    void updateGrid();
    void updateHex(HexDirection dir);

    HexPoint inject(Vector2 point);
    Vector2 project(HexPoint);

    void resize(int width, int height) override;

    HexPoint hexAdd(HexPoint a, HexPoint b) const;
    HexPoint hexSubtract(HexPoint a, HexPoint b) const;
    HexPoint hexMultiply(HexPoint a, int k) const;
    int hexLength(HexPoint a) const;
    int hexDistance(HexPoint a, HexPoint b) const;
    HexPoint hexNeighbor(HexPoint hex, HexDirection dir) const;
    // void transition(State::Screen);
    // void unload();
};
