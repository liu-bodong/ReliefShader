#pragma once

#include <QString>
#include <vector>

class Vertex;
class Face;

/// <summary>
/// Data class stores the raw data of the 3D model.
/// </summary>
class Data
{
public:
    static inline Data* GetInstance()
    {
        return g_pInstance;
    }

    double GetMinX() const { return m_minX; };
    double GetMinY() const { return m_minY; };
    double GetMinValue() const { return m_minZ; };

    // returns aspect ratio (width / height)
    double GetAspectRatio() const { return m_aspectRatio; };
    double GetOriginalWidth() const { return m_originalWidth; };
    double GetOriginalHeight() const { return m_originalHeight; };
    double GetValueRange() const { return m_valueRange; };

    Vertex* GetVertex(int index) const { return m_vertices[index]; };
    Face* GetFace(int index) const { return m_faces[index]; };

    unsigned int GetVerticesCount() const { return unsigned int(m_vertices.size()); };
    unsigned int GetFacesCount() const { return unsigned int(m_faces.size()); };

    std::vector<Vertex*>* GetVertices() { return &m_vertices; };
    std::vector<Face*>* GetFaces() { return &m_faces; };

    bool LoadFile(const QString&);


private:
    Data() = default;
    ~Data() = default;

    void ClearData();
    void AddVertex(Vertex* vertex) { m_vertices.push_back(vertex); }
    void AddFace(Face* face) { m_faces.push_back(face); }

    static Data* g_pInstance;

    double m_maxX;            // max X
    double m_minX;
    double m_maxY;
    double m_minY;
    double m_maxZ;
    double m_minZ;

    double m_aspectRatio;     // Width / Height��     ��߱�

    double m_originalWidth;   // maxX - minX��        ԭʼ���
    double m_originalHeight;  // maxY - minY��        ԭʼ�߶�
    double m_valueRange;      // max - min��          �̷߳�Χ

    int m_numVertices;        // number of vertices�� ��������
    int m_numFaces;           // number of faces��    ������

    std::vector<Vertex*> m_vertices;     // all vertices�� ���еĶ���
    std::vector<Face*> m_faces;          // all faces��    ���е���
};

