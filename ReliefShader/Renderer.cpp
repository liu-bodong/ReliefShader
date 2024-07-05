#include "Renderer.h"
#include "Vertex.h"
#include "Face.h"
#include "Line.h"

#include <QFileDialog>
#include <qfile.h>
#include <qtextstream.h>
#include <cmath>
#include <set>


Renderer* Renderer::g_pInstance = new (std::nothrow) Renderer();

// #################### public ####################


/// <summary>
/// To be implemented
/// </summary>
void Renderer::Render()
{
    Clear();


    // create empty contours corresponding to the number of contours
    for (auto i = 0; i < m_numContours; ++i)
    {
        auto set = std::set<Line>();
        m_contours.push_back(set);
    }

    for (auto face : m_faces)
    {
        CalculateIntersectionsOnFace(face);
    }

    lock = false;

}

void Renderer::CalculateIntersectionsOnFace(Face* face)
{
    // get the vertices
    auto iV1 = face->GetAt(0);
    auto iV2 = face->GetAt(1);
    auto iV3 = face->GetAt(2);

    std::vector<Line> sides; // sides on the intersection of contours and faces
    sides.push_back(Line(iV1, iV2));
    sides.push_back(Line(iV2, iV3));
    sides.push_back(Line(iV3, iV1));


    for (auto i = 0; i < m_numContours; ++i)
    {
        auto contourValue = m_contourValues[i];
        std::vector<Vertex*> intersections;

        // process each side of three sides of the face
        for (Line side : sides)
        {
            if (side.count == 2) { continue; } // skip if the side has been processed

            auto p = LinearInterpolate(m_vertices[side.first], m_vertices[side.second], contourValue);

            if (p)
            {
                intersections.push_back(p);
                m_charSides.insert(side);
                side.count = side.count + 1;
            }
        }

        if (intersections.size() == 2)
        {
            RecordCharacteristic(intersections.at(0), intersections.at(1), i);
        }
    }
}

/// <summary>
/// Records the characteristic vertices in the set "intersections".
/// </summary>
/// <param name="intersections"></param>
void Renderer::RecordCharacteristic(Vertex* v1, Vertex* v2, int contour)
{
    // record the characteristic vertices
    if (v1 && v2)
    {
        m_charVertices.push_back(v1);
        m_charVertices.push_back(v2);
        m_charLines.insert(Line(int(m_charVertices.size() - 2), int(m_charVertices.size() - 1)));
        m_contours[contour].insert(Line(int(m_charVertices.size() - 2), int(m_charVertices.size() - 1)));
    }
}
/// <summary>
/// Calculates the intersection point between two vertices and a given z value.
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <param name="z"></param>
/// <returns>
/// A new Vertex object representing the intersection point.
/// Or nullptr if the intersection point does not exist.
/// </returns>
Vertex* Renderer::LinearInterpolate(Vertex* v1, Vertex* v2, double z)
{
    // calculate the intersection point
    auto x1 = v1->GetX();
    auto y1 = v1->GetY();
    auto z1 = v1->GetZ();

    auto x2 = v2->GetX();
    auto y2 = v2->GetY();
    auto z2 = v2->GetZ();

    if ((z1 - z) * (z2 - z) < 0) // means the intersection point exists
    {
        // calculate the intersection point
        auto x = (x1 + (z - v1->GetZ()) * (x2 - x1) / (v2->GetZ() - v1->GetZ()));
        auto y = (y1 + (z - v1->GetZ()) * (y2 - y1) / (v2->GetZ() - v1->GetZ()));
        return new Vertex(x, y, z);
    }
    else if (z1 == z)
    {
        v1->SetZ(z1 - 1E-15);
        auto x = (x1 + (z - v1->GetZ()) * (x2 - x1) / (v2->GetZ() - v1->GetZ()));
        auto y = (y1 + (z - v1->GetZ()) * (y2 - y1) / (v2->GetZ() - v1->GetZ()));
        return new Vertex(x, y, z);
    }
    else if (z2 == z)
    {
        v2->SetZ(z2 - 1E-15);
        auto x = (x1 + (z - v1->GetZ()) * (x2 - x1) / (v2->GetZ() - v1->GetZ()));
        auto y = (y1 + (z - v1->GetZ()) * (y2 - y1) / (v2->GetZ() - v1->GetZ()));
        return new Vertex(x, y, z);
    }
    return nullptr;
}

/// <summary>
/// Clears the Renderer.
/// </summary>
void Renderer::Clear()
{
    m_charVertices.clear();
    m_charLines.clear();
    m_charSides.clear();
    m_contours.clear();
}

/// <summary>
/// Loads a file with the given fileName and populates the Renderer with vertices and faces.
/// </summary>
/// <param name="fileName">The name of the file to load.</param>
/// <returns>
/// bool: true if loaded successfully
/// </returns>
bool Renderer::LoadFile(const QString& fileName)
{
    // initialize max and min X, Y, Z values
    double maxX = 0;
    double minX = DBL_MAX;
    double maxY = 0;
    double minY = DBL_MAX;
    double maxZ = 0;
    double minZ = DBL_MAX;

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

            maxX = std::max(maxX, x);// get max and min X, Y, Z values when loading files
            minX = std::min(minX, x);
            maxY = std::max(maxY, y);
            minY = std::min(minY, y);
            maxZ = std::max(maxZ, z);
            minZ = std::min(minZ, z);
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
    m_maxZ = maxZ;
    m_minZ = minZ;
    m_maxX = maxX;
    m_minX = minX;
    m_maxY = maxY;
    m_minY = minY;
    m_numVertices = numVertices;
    m_numFaces = numFaces;
    m_originalWidth = m_maxY - m_minY; // calculate original size
    m_valueRange = m_maxZ - m_minZ;
    m_originalHeight = m_maxX - m_minX;
    m_aspectRatio = m_originalWidth / m_originalHeight;
    m_valueRange = m_maxZ - m_minZ;
    return true;
}

/// <summary>
/// Saves the settings of the renderer to a file of name "fileName".
/// </summary>
/// <param name="fileName"></param>
/// <returns>bool: true if saved successfully</returns>
bool Renderer::SaveSettings(const QString& fileName) const
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << "MapResolution " << m_resolution << "\n";
    out << "Interval " << m_interval << "\n";
    out << "NumContours " << m_numContours << "\n";
    out << "UseGradient " << m_useGradient << "\n";
    file.close();
    return true;
}

/// <summary>
/// Loads the settings of the renderer from a file of name "fileName".
/// </summary>
/// <param name="fileName"></param>
/// <returns>bool: true if loaded successfully</returns>
bool Renderer::LoadSettings(const QString& fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString line;
    while (!file.atEnd())
    {
        line = file.readLine();
        QString word = FirstWord(line);
        if (word == "MapResolution")
        {
            m_resolution = line.split(' ')[1].toDouble();
        }
        else if (word == "Interval")
        {
            m_interval = line.split(' ')[1].toDouble();
        }
        else if (word == "NumContours")
        {
            m_numContours = line.split(' ')[1].toInt();
        }
        else if (word == "UseGradient")
        {
            m_useGradient = line.split(' ')[1].toInt();
        }
    }
    file.close();
    return true;
}

// Synchronize the rendered height with the rendered width.
void Renderer::SyncHeight()
{
    m_renderedHeight = static_cast<int>(m_renderedWidth / m_aspectRatio);
}

// Synchronize the rendered width with the rendered height.
void Renderer::SyncWidthUsingHeight()
{
    m_renderedWidth = static_cast<int>(m_renderedHeight * m_aspectRatio);
}

// Synchronize the rendered width with the resolution.
void Renderer::SyncWidthUsingResolution()
{
    m_renderedWidth = static_cast<int>(m_originalWidth / m_resolution);
}

// Synchronize the resolution with the rendered width.
void Renderer::SyncResolution()
{
    m_resolution = m_originalWidth / m_renderedWidth;
}

// Synchronize the interval with the number of contours.
void Renderer::SyncInterval()
{
    if (m_numContours != 0)
    {
        m_interval = m_valueRange / m_numContours;
    }
}

// Synchronize the number of contours with the interval.
void Renderer::SyncNumContours()
{
    if (m_interval != 0)
    {
        // round up to the nearest integer
        m_numContours = static_cast<int>(ceil(m_valueRange / m_interval));
    }
}

void Renderer::SyncContours()
{
    if (m_interval != 0 && m_numContours != 0)
    {
        m_contourValues.clear();
        for (int i = 0; i < m_numContours; ++i)
        {
            m_contourValues.push_back(m_minZ + (i * m_interval));
        }
    }
}

// #################### private ####################

/// <summary>
/// Returns the first word in a given side.
/// </summary>
/// <param name="side"></param>
/// <returns>QString: the first word in the "side"</returns>
QString Renderer::FirstWord(const QString& line) const
{
    QString word;
    for (auto c : line)
    {
        if (c == ' ')
            break;
        word += c;
    }
    return word;
}

