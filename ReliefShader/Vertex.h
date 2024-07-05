#pragma once

#include <QObject>
#include <QColor>

class Vertex : public QObject
{

public:
    Vertex(double x, double y, double z)
        : m_x(x), m_y(y), m_z(z) {};
    ~Vertex() = default;

    double GetX() const { return m_x; };
    double GetY() const { return m_y; };
    double GetZ() const { return m_z; };

    void SetX(double x) { m_x = x; };
    void SetY(double y) { m_y = y; };
    void SetZ(double z) { m_z = z; };

private:
    double m_x;
    double m_y;
    double m_z;
    QColor m_color = nullptr;
};
