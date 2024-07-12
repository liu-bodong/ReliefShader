#pragma once

#include <vector>

class Side;

class Face
{
public:
    Face() = default;
    Face(int v1, int v2, int v3) : m_vertices{ v1, v2, v3 } {};
    ~Face() = default;

    void AddVertex(int index) { if (m_vertices.size() < 3) m_vertices.push_back(index); }
    std::vector<int> GetVertices() const { return m_vertices; }
    int Get(int index) const { return (0 <= index && index < 3) ? m_vertices.at(index) : -1; }

private:
    std::vector<int> m_vertices;
};
