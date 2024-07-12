#pragma once
#include <cmath>

class Vertex;
struct DirectedLine;

//
struct Vector
{
public:
    Vector() = default;
    Vector(double x, double y)
        : x(x), y(y) {};
    ~Vector() = default;

    Vector(const Vector& v) = default;
    explicit Vector(const Vector* v);

    // Constructs a vector from a line
    Vector(const DirectedLine* line);

    Vector(const Vector& v1, const Vector& v2)
        : x(v2.x - v1.x), y(v2.y - v1.y) {};

    // Constructs a vector [b - a] from two vertices
    Vector(const Vertex* a, const Vertex* b);
    explicit Vector(const Vertex*);

    // Returns the length of this vector
    double Length() inline const { return std::sqrt(x * x + y * y); };

    Vector* Add(const Vector& v) inline const { return new Vector(x + v.x, y + v.y); };
    Vector* Subtract(const Vector& v) inline const { return new Vector(x - v.x, y - v.y); };
    Vector* Multiply(double s) inline const { return new Vector(x * s, y * s); };
    // Returns the dot product of this vector and vector v
    double Dot(const Vector& v) inline const { return x * v.x + y * v.y; };

    // Returns the z component of the cross product of this vector and vector v
    double Cross(const Vector& v) const;

    double x = 0;
    double y = 0;
};

