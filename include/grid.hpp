#pragma once

#include "window.hpp"
#include "types.hpp"

#include <raylib.h>

#include <unordered_map>
#include <array>

#define HEX_SIZE 60.0f
// Hex grid extent (half width/height) - 1 (center hex)
// or number of onion hex layers around center hex
#define GRID_SIZE 2

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

class Grid : public Layer {
  std::unordered_map<HexPoint, HexState> map;

  // flat top
  const Matrix2x2Pair view = Matrix2x2Pair({
    3.0f/2.0f, 0.0f, sqrtf(3.0f)/2.0f, sqrtf(3.0f),
    2.0f/3.0f, 0.0f, -1.0f/3.0f, sqrtf(3.0f)/3.0f
  });
  // pointy top
  // const Matrix2x2Pair view = Matrix2x2Pair({
  //   sqrtf(3.0f), sqrtf(3.0f)/2.0f, 0.0f, 3.0f/2.0f,
  //   sqrtf(3.0f)/3.0f, -1.0f/3.0f, 0.0, 2.0f/3.0f
  // });

  const std::array<HexPoint, 6> directions = {
    HexPoint(0, -1, 1), HexPoint(1, -1, 0), HexPoint(1, 0, -1),   
    HexPoint(0, 1, -1), HexPoint(-1, 1, 0), HexPoint(-1, 0, 1)
  };

  const Window& window;
  Vector2 origin = { window.halfWidthf, window.halfHeightf };

  Color bgColor = BEIGE;
  Color borderColor = BLACK;
  // NOTE: hexSize could be a vector2 to skew the hexes
  // needs a custom draw function that draws each hex
  // and update inject/project accordingly
  float hexSize = HEX_SIZE;
  int gridSize = GRID_SIZE;

public:
    Grid(const Window& window): window(window) {};
    ~Grid() = default;

    void load();
    void generate(int layers);
    void reset();

    void render() const;
    void renderHex(const HexState&) const;

    void update();
    void updateHex(Direction);
    void phaseChange(int phase);

    HexPoint inject(Vector2 point);
    Vector2 project(HexPoint);

    void resize(int width, int height) override;

    HexPoint hexAdd(HexPoint, HexPoint) const;
    HexPoint hexSubtract(HexPoint, HexPoint) const;
    HexPoint hexMultiply(HexPoint, int k) const;
    int hexLength(HexPoint) const;
    int hexDistance(HexPoint, HexPoint) const;
    HexPoint hexNeighbor(HexPoint, Direction) const;
    Vector2 hexPosition(int q, int r, int s) const;
    Vector2 hexPosition(HexPoint) const;
    HexPoint hexUnitRotate(HexPoint, bool clockwise) const;
    HexPoint hexCorner(HexPoint unitHex) const;

    HexPoint hexFindFirstEmpty() const;
    HexPoint hexFindCenterEmpty() const;
    HexPoint hexFindRandomEmpty() const;
    void occupy(HexPoint, Effigy);
    void vacate(HexPoint);
    Effigy getEffigy(HexPoint) const;
    int getTotalHexes() const;

    void clear();
    bool isFull() const;
    bool isEdge(HexPoint, Direction) const;
    bool isHexEdge(HexPoint) const;
    bool isOccupied(HexPoint) const;
    bool isValid(HexPoint) const;
    // void transition(State::Screen);
    // void unload();
};
