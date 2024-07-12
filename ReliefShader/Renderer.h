#pragma once

#include <QObject>
#include <QVector>
#include <float.h>
#include <set>
#include <vector>
#include <list>
#include <utility>
#include "Line.h"
#include "ColorScheme.h"
#include <memory>

class Vertex;
class Face;
struct Vector;
struct DirectedLine;


// Renderer
// 一个单例类，用于渲染图像
// 该类的实例是一个单例，可以通过GetInstance()函数获取
// 
class Renderer
{
public:
    // Side is a pair of two vertex indices
    using Side = std::pair<int, int>;

    // Returns the instance of the Renderer class
    static inline Renderer* GetInstance()
    {
        return g_pInstance;
    }

    bool IsLocked() const { return lock; }
    void Lock() { lock = true; }
    void UnLock() { lock = false; }

    ColorScheme::RGB GetColorAt(double x, double y) const;

    ColorScheme::RGB InterpolateColor(ColorScheme::RGB color1, ColorScheme::RGB color2, double ratio) const;

    std::vector<ColorScheme::RGB> GetContourColors() const { return m_contourColors; };

    std::vector<double> GetStepValues() const { return m_contourVals; };
    std::vector< std::set<DirectedLine*> > GetContourLines() const { return m_contours; };

    // Synchronizes the contours with the current number of contours and intervals
    void SyncContours();

    // save & load
    bool SaveSettings(const QString&) const;
    bool LoadSettings(const QString&);

    void Render();


private:
    Renderer() = default;
    ~Renderer() = default;

    static Renderer* g_pInstance;

    bool lock = true;

    //################## Members for Rendering 用于渲染的数据成员 ##############################

    std::vector<double> m_contourVals;                    // contour values from lowest to highest 从低到高的等高线值
    std::vector<ColorScheme::RGB> m_contourColors;        // colors for each contour line 每个等高线的颜色
    std::vector< std::set<DirectedLine*> > m_contours;    // contour lines for each contour value 每个等高线值的等高线


    //################### Helper Functions 辅助函数 ##########################################
    Vector* LinearInterpolate(Vertex*, Vertex*, double);
    void ProcessFace(const Face*);
    void RecordLine(std::vector<Vector*>, std::vector<int>, double, double, double, int);
    void Clear();
    void GenerateContourColors();

    std::tuple<int, DirectedLine*, Vector> FindNearestContourLine(double x, double y) const;
    Vector LineToPointVector(const Vector& p, const Vector& v1, const Vector& v2) const;
    std::pair<DirectedLine*, Vector> FindNearestLine(double x, double y, int contourIndex) const;
};