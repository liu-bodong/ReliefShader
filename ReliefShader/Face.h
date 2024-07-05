#pragma once

#include <QObject>
#include <QVector>

class Vertex;

class Face
{
public:
    Face() = default;
    Face(int v1, int v2, int v3)
        : m_vertices{ v1, v2, v3 } {};
    ~Face() = default;

    void AddVertex(int index) { if (m_vertices.size() < 3)m_vertices.push_back(index); }
    int GetAt(int index)
    {
        if (0 <= index && index <= 2) return m_vertices[index];
        else return -1;
    }


private:
    // make a vector
    QVector<int> m_vertices;
};
