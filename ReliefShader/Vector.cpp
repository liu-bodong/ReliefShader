#include "Vector.h"
#include "Vertex.h"
#include "Line.h"

Vector::Vector(const Vertex* v1, const Vertex* v2) :
    x(v2->GetX() - v1->GetX()),
    y(v2->GetY() - v1->GetY())
{}

Vector::Vector(const Vector* v) :
    x(v->x),
    y(v->y)
{}

Vector::Vector(const DirectedLine* line) :
    x(line->b->x - line->a->x),
    y(line->b->y - line->a->y)
{}

Vector::Vector(const Vertex* v) :
    x(v->GetX()),
    y(v->GetY())
{}

double Vector::Cross(const Vector& v) const
{
    return (x * v.y - y * v.x);
}