#pragma once


class Vertex
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

    double Dot(const Vertex& v) const { return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z; };

    Vertex* Cross(const Vertex& v) const
    {
        return new Vertex(m_y * v.m_z - m_z * v.m_y,
            m_z * v.m_x - m_x * v.m_z,
            m_x * v.m_y - m_y * v.m_x);
    }

private:
    double m_x;
    double m_y;
    double m_z;

};
