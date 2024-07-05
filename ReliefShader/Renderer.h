#pragma once

#include <QObject>
#include <QVector>
#include <float.h>
#include <set>
#include <vector>
#include <utility>
#include "Line.h"

class Vertex;
class Face;

class ColorScheme;


// RendererClass
// Summary:
// 
// Key Terms:
// 
class Renderer
{
public:

    static Renderer* GetInstance()
    {
        return g_pInstance;
    }

    bool IsLocked() const { return lock; }
    void Lock() { lock = true; }
    void UnLock() { lock = false; }

    void SetRenderedHeight(int h) { m_renderedHeight = h; };
    void SetRenderedWidth(int w) { m_renderedWidth = w; };
    void SetResolution(double r) { m_resolution = r; };
    void SetInterval(double i) { m_interval = i; };
    void SetNumContours(int n) { m_numContours = n; };
    void SetUseGradient(bool b) { m_useGradient = b; };
    void SetColorScheme(ColorScheme* c) { m_colorScheme = c; };

    int GetRenderedHeight() const { return m_renderedHeight; };
    int GetRenderedWidth() const { return m_renderedWidth; };
    double GetOriginalWidth() const { return m_originalWidth; };
    double GetOriginalHeight() const { return m_originalHeight; };
    double GetResolution() const { return m_resolution; };
    double GetInterval() const { return m_interval; };
    int GetNumContours() const { return m_numContours; };
    bool GetUseGradient() const { return m_useGradient; };
    double GetXOffset() const { return m_minX; };
    double GetYOffset() const { return m_minY; };
    ColorScheme* GetColorScheme() const { return m_colorScheme; };


    std::vector<double> GetStepValues() const { return m_contourValues; };
    std::set<Line> GetCharLines() const { return m_charLines; };
    std::vector<Vertex*> GetCharVertices() const { return m_charVertices; };
    std::vector< std::set<Line> > GetContourLines() const { return m_contours; };


    void SyncHeight();
    void SyncWidthUsingHeight();
    void SyncWidthUsingResolution();
    void SyncResolution();
    void SyncInterval();
    void SyncNumContours();
    void SyncContours();

    // save & load
    bool LoadFile(const QString&);
    bool SaveSettings(const QString&) const;
    bool LoadSettings(const QString&);

    void Render();


private:
    Renderer() = default;
    ~Renderer() = default;

    static Renderer* g_pInstance;

    bool lock = true;

    // for rendering

    // contour values from lowest to highest
    std::vector<double> m_contourValues;

    // The following share the same index with m_vertices
    // 以下的索引指向m_vertices
    std::vector<Vertex*> m_vertices;       // all vertices 所有的顶点
    std::vector<Face*> m_faces;            // all faces    所有的面
    std::set<Line> m_charSides;            // a set of characteristic face sides that intersect the contour lines, each line is a pair of two vertices' index in m_vertices

    // The following share the same index with m_charVertices
    // 以下的索引指向m_charVertices
    std::vector<Vertex*> m_charVertices;   // vertices on the intersection of contours and faces' lines 特征顶点：在等高线和面的交线上的顶点
    std::set<Line> m_charLines;            // a set of characteristic lines of faces, each line is a pair of two characteristic vertices' index in m_charVertices
    std::vector< std::set<Line> > m_contours;// a list of contours, each contour is a set of characteristic lines

    // relief properties
    int m_renderedHeight = 1080;
    int m_renderedWidth;
    double m_resolution;
    double m_interval;
    int m_numContours = 10;
    bool m_useGradient = false;
    ColorScheme* m_colorScheme = nullptr;

    // original file properties
    double m_maxX;
    double m_minX;
    double m_maxY;
    double m_minY;
    double m_maxZ;
    double m_minZ;
    double m_aspectRatio;     // X / Y; Width / Height
    double m_originalWidth;   // maxX - minX
    double m_originalHeight;  // maxY - minY
    double m_valueRange;      // max - min
    int m_numVertices;        // number of vertices
    int m_numFaces;           // number of faces



    QString FirstWord(const QString& line) const;

    void AddVertex(Vertex* vertex) { m_vertices.push_back(vertex); }
    void AddFace(Face* face) { m_faces.push_back(face); }

    Vertex* LinearInterpolate(Vertex*, Vertex*, double);
    void CalculateIntersectionsOnFace(Face*);

    void RecordCharacteristic(Vertex* v1, Vertex* v2, int);

    void Clear();
};
