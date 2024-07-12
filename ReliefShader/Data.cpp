#include "Data.h"

#include <qfiledialog.h>
#include <qfile.h>
#include <vector>
#include <qdebug.h>
#include <utility>

#include "Vertex.h"
#include "Face.h"

Data* Data::g_pInstance = new (std::nothrow) Data();

// Loads a file and stores information in the Data class.
bool Data::LoadFile(const QString& fileName)
{
    // initialize max and min X, Y, Z values
    ClearData();

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString line;
    int numVertices = 0;
    int numFaces = 0;
    while (!file.atEnd())
    {
        line = file.readLine();
        QStringList list = line.split(' ');
        if (list[0] == "v")
        {
            double x = list[1].toDouble();
            double y = list[2].toDouble();
            double z = list[3].toDouble();
            auto vertex = new Vertex(x, y, z);
            AddVertex(vertex);

            m_maxX = std::max(m_maxX, x);// get max and min X, Y, Z values when loading files
            m_minX = std::min(m_minX, x);
            m_maxY = std::max(m_maxY, y);
            m_minY = std::min(m_minY, y);
            m_maxZ = std::max(m_maxZ, z);
            m_minZ = std::min(m_minZ, z);
        }
        else if (list[0] == "f")
        {
            int v1 = ((list[1].split('/'))[0]).toInt();
            int v2 = ((list[2].split('/'))[0]).toInt();
            int v3 = ((list[3].split('/'))[0]).toInt();
            auto face = new Face(v1 - 1, v2 - 1, v3 - 1);
            AddFace(face);
        }
        else if (list[0] == "#")
        {
            if (list[2].trimmed() == "vertices")
            {
                numVertices = list[1].toInt();
            }
            else if (list[2].trimmed() == "faces")
            {
                numFaces = list[1].toInt();
            }
        }
    }
    file.close();

    if (numVertices != m_vertices.size() || numFaces != m_faces.size())
    {
        m_vertices.clear(); // clean up if the number of vertices or faces is not correct
        m_faces.clear();
        return false;
    }

    // initialize the Renderer
    m_numVertices = numVertices;
    m_numFaces = numFaces;
    m_originalWidth = m_maxY - m_minY; // calculate original size
    m_valueRange = m_maxZ - m_minZ;
    m_originalHeight = m_maxX - m_minX;
    m_aspectRatio = m_originalWidth / m_originalHeight;
    m_valueRange = m_maxZ - m_minZ;
    return true;
}

// Clears all data.
void Data::ClearData()
{
    m_maxX = std::numeric_limits<double>::min();
    m_minX = std::numeric_limits<double>::max();
    m_maxY = std::numeric_limits<double>::min();
    m_minY = std::numeric_limits<double>::max();
    m_maxZ = std::numeric_limits<double>::min();
    m_minZ = std::numeric_limits<double>::max();
    m_aspectRatio = 0;
    m_originalWidth = 0;
    m_originalHeight = 0;
    m_valueRange = 0;
    m_numVertices = 0;
    m_numFaces = 0;
    m_vertices.clear();
    m_faces.clear();
}