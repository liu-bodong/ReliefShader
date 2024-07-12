#pragma once

#include <memory>

struct Vector;

// DirectedLine class stores two vectors [a] and [b] that represent a line segment [b - a].
// The direction of the line is from [a] to [b], and it is required that the right
// hand side normal of the line must point towards the higher contour.
struct DirectedLine
{
    DirectedLine(Vector* a, Vector* b) : a(a), b(b) {};
    DirectedLine() = delete;

    Vector* a;
    Vector* b;

    // implement comparison operators
    bool operator==(const DirectedLine& other) const { return a == other.a && b == other.b; }
    bool operator!=(const DirectedLine& other) const { return !(*this == other); }
};

