#include <QFileDialog>
#include <qfile.h>
#include <set>
#include <vector>
#include <qdebug.h>
#include <utility>
#include <memory>
#include <stdexcept>

#include "Renderer.h"
#include "Vertex.h"
#include "Face.h"
#include "Line.h"
#include "ColorScheme.h"
#include "Vector.h"
#include "Data.h"
#include "Settings.h"

Renderer* Renderer::g_pInstance = new (std::nothrow) Renderer();

// #################### public ####################

/// <summary>
/// To be implemented
/// </summary>
void Renderer::Render()
{
    // Init
    if (!Settings::GetInstance()) { return; }
    if (!Data::GetInstance()) { return; }

    auto pData = Data::GetInstance();
    auto pSettings = Settings::GetInstance();
    int numContours = pSettings->GetNumContours();
    Clear();

    // create empty contours
    for (int i = 0; i < numContours; ++i)
    {
        auto set = std::set<DirectedLine*>();
        m_contours.push_back(set);
    }

    // process each face
    for (unsigned int i = 0; i < pData->GetFacesCount(); i++)
    {
        auto face = pData->GetFace(i);
        ProcessFace(face);
    }

    UnLock();
}

// 目前这个函数的实现是有问题的
// 1. 没有考虑到边界情况，边界外的点也会被上色
// 2. 边界内的点上色异常
ColorScheme::RGB Renderer::GetColorAt(double x, double y) const
{
    auto contour1 = FindNearestContourLine(x, y);
    auto contourLineSeg = std::get<1>(contour1);
    int matchedContourIndex = std::get<0>(contour1);
    auto normalVector = std::get<2>(contour1);
    auto distanceToMatchedContour = normalVector.Length();

    auto lineVector = (contourLineSeg->b)->Subtract(*contourLineSeg->a);

    auto contourLineSegDirection = Vector(lineVector);

    int secondContourIndex;

    if (normalVector.Cross(contourLineSegDirection) > 0) // the normal vector is pointing towards the higher contour
    {
        secondContourIndex = matchedContourIndex + 1;
    }
    else // the normal vector is pointing towards the lower contour
    {
        secondContourIndex = matchedContourIndex - 1;
    }

    try
    {
        auto contour2 = FindNearestLine(x, y, secondContourIndex);
        auto normalVector2 = contour2.second;
        auto distanceToSecondContour = normalVector2.Length();

        auto ratio = distanceToMatchedContour / (distanceToMatchedContour + distanceToSecondContour);

        auto color1 = m_contourColors[matchedContourIndex];
        auto color2 = m_contourColors[secondContourIndex];

        return InterpolateColor(color1, color2, ratio);
    }
    catch (const std::out_of_range&)
    {
        return m_contourColors[matchedContourIndex];
    }
}



// Find the nearest contour line to the given point (x, y). Returns the contour index, the contour line, and the vector from the line pointing to the point.
// 找到距离给定点(x, y)最近的等高线。返回等高线的索引、等高线和指向点的向量。
std::tuple<int, DirectedLine*, Vector> Renderer::FindNearestContourLine(double x, double y) const
{
    double distance = std::numeric_limits<double>::max(); // set distance to max double
    double contourIndex = -1; // initialize contourIndex to -1
    Vector normalVector;
    DirectedLine* contourLine = nullptr; // initialize contourLine to nullptr
    for (int k = 0; k < m_contours.size(); k++) // iterate over each contour
    {
        auto pair = FindNearestLine(x, y, k);
        auto tempNormalVector = pair.second;
        contourLine = pair.first;
        if (normalVector.Length() < distance)
        {
            distance = normalVector.Length();
            contourIndex = k;
            normalVector = tempNormalVector;
        }
    }
    return std::make_tuple(contourIndex, contourLine, normalVector);
}

// Find the nearest line to the given point (x, y) in the contour with the given index. Returns the line and the vector from the line pointing to the point.
// 在给定索引的等高线中找到距离给定点(x, y)最近的线。返回线和指向点的向量。
std::pair<DirectedLine*, Vector> Renderer::FindNearestLine(double x, double y, int contourIndex) const
{
    if (contourIndex < 0 || contourIndex >= m_contours.size()) // contourIndex out of range
    {
        throw std::out_of_range("Contour index out of range.");
    }

    double distance = std::numeric_limits<double>::max(); // set distance to max double
    Vector normalVector;
    DirectedLine* contourLine = nullptr; // Initialize contourLine to nullptr
    for (auto line : m_contours.at(contourIndex)) // iterate over each line in the contour
    {
        auto p = Vector(x, y);
        auto tempVector = LineToPointVector(p, *(line->a), *(line->b));
        if (tempVector.Length() < distance)
        {
            distance = normalVector.Length();
            contourLine = line;
            normalVector = tempVector;
        }
    }
    return std::make_pair(contourLine, normalVector);
}

// Returns the vector from a point to a line segment. The length of the vector is the distance from the point to the line segment.
// 返回从点到线段的向量。向量的长度是点到线段的距离。
Vector Renderer::LineToPointVector(const Vector& p, const Vector& v1, const Vector& v2) const
{
    // creates a vector from vector v1 to vector v2
    auto a = std::make_shared<Vector>(v1, v2);
    auto b = std::make_shared<Vector>(v1, p);
    auto a_dot_b = a->Dot(*b);
    if (a_dot_b < 0) // projection is outside the line segment, closer to v1
    {
        return Vector(v1, p);
    }
    else if (a_dot_b > a->Length()) // projection is outside the line segment, closer to v2
    {
        return Vector(v2, p);
    }
    else // projection is inside the line segment
    {
        auto b_projectionOn_a = std::make_shared<Vector>(a->Multiply(a_dot_b / a->Length()));
        auto normalVector = std::make_shared<Vector>(b->Subtract(*b_projectionOn_a));
        return *normalVector;
    }
}

void Renderer::ProcessFace(const Face* face)
{
    auto pSettings = Settings::GetInstance();
    auto pData = Data::GetInstance();

    auto vertices = pData->GetVertices();
    auto numContours = pSettings->GetNumContours();

    auto v1Ind = face->Get(0);
    auto v2Ind = face->Get(1);
    auto v3Ind = face->Get(2);

    auto z1 = pData->GetVertex(v1Ind)->GetZ();
    auto z2 = pData->GetVertex(v2Ind)->GetZ();
    auto z3 = pData->GetVertex(v3Ind)->GetZ();

    auto zMin = std::min(z1, std::min(z2, z3));
    auto zMax = std::max(z1, std::max(z2, z3));

    auto side12 = Side(v1Ind, v2Ind);
    auto side23 = Side(v2Ind, v3Ind);
    auto side31 = Side(v3Ind, v1Ind);
    std::vector<Side> sides = { side12, side23, side31 };

    // find the contour values that fall within the range
    std::vector<int> hitContourVals;
    for (auto i = 0; i < numContours; ++i)
    {
        auto contourValue = m_contourVals[i];
        if (zMin <= contourValue && contourValue <= zMax)
        {
            hitContourVals.push_back(i);
        }
    }

    // for each contour value in the range
    for (auto contourIndex : hitContourVals)
    {
        auto contourValue = m_contourVals[contourIndex];
        std::vector<Vector*> intersections;
        std::vector<int> intersectSides;

        for (auto i = 0; i < 3; ++i) // process each side by order, 0-1, 1-2, 2-0
        {
            auto v1 = vertices->at(sides[i].first);
            auto v2 = vertices->at(sides[i].second);
            auto intersection = LinearInterpolate(v1, v2, contourValue);
            if (intersection)
            {
                intersections.push_back(intersection);
                intersectSides.push_back(i);
            }
        }

        RecordLine(intersections, intersectSides, z1, z2, z3, contourIndex);
    }
}

// Records a line between two vertices and stores the contour index.
void Renderer::RecordLine(std::vector<Vector*> intersections, std::vector<int> intersectSides, double z1, double z2, double z3, int contourIndex)
{
    if (intersections.size() == 2)
    {
        auto contourValue = m_contourVals[contourIndex];
        auto sidePair = std::make_pair(intersectSides[0], intersectSides[1]);
        DirectedLine* line;
        if (sidePair == std::make_pair(0, 1))
        {
            if (z2 > contourValue) { line = new DirectedLine(intersections[0], intersections[1]); }
            else { line = new DirectedLine(intersections[1], intersections[0]); }
        }
        else if (sidePair == std::make_pair(1, 2))
        {
            if (z3 > contourValue) { line = new DirectedLine(intersections[0], intersections[1]); }
            else { line = new DirectedLine(intersections[1], intersections[0]); }
        }
        else if (sidePair == std::make_pair(0, 2))
        {
            if (z3 > contourValue) { line = new DirectedLine(intersections[0], intersections[1]); }
            else { line = new DirectedLine(intersections[1], intersections[0]); }
        }
        else
        {
            // should not happen
            return;
        }
        m_contours[contourIndex].insert(line);
    }
}

/// <summary>
/// Calculates for an intersection point between two vertices and a given z value.
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <param name="z"></param>
/// <returns>
/// A smart pointer to a Vector object that represents the intersection point.
/// </returns>
Vector* Renderer::LinearInterpolate(Vertex* v1, Vertex* v2, double z)
{
    // calculate the intersection point
    auto x1 = v1->GetX();
    auto y1 = v1->GetY();
    auto z1 = v1->GetZ();

    auto x2 = v2->GetX();
    auto y2 = v2->GetY();
    auto z2 = v2->GetZ();

    if (z1 == z) {
        v1->SetZ(z1 - 1E-10);
    }
    else if (z2 == z) {
        v2->SetZ(z2 - 1E-10);
    }

    if ((z1 - z) * (z2 - z) < 0) // means the intersection point exists
    {
        // calculate the intersection point
        auto x = (x1 + (z - z1) * (x2 - x1) / (z2 - z1));
        auto y = (y1 + (z - z1) * (y2 - y1) / (z2 - z1));
        return new Vector(x, y);
    }
    return nullptr;
}

// Interpolates the color between two colors according to the given ratio.
// Formula: color = color1 + (color2 - color1) * ratio
ColorScheme::RGB Renderer::InterpolateColor(ColorScheme::RGB color1, ColorScheme::RGB color2, double ratio) const
{
    auto r = static_cast<short>(std::get<0>(color1) + (std::get<0>(color2) - std::get<0>(color1)) * ratio);
    auto g = static_cast<short>(std::get<1>(color1) + (std::get<1>(color2) - std::get<1>(color1)) * ratio);
    auto b = static_cast<short>(std::get<2>(color1) + (std::get<2>(color2) - std::get<2>(color1)) * ratio);

    return ColorScheme::RGB(r, g, b);
}

// Synchronizes the contours of the image.
void Renderer::SyncContours()
{
    auto pSettings = Settings::GetInstance();
    if (pSettings->GetInterval() != 0 && pSettings->GetNumContours() != 0)
    {
        m_contourVals.clear();
        for (int i = 0; i < pSettings->GetNumContours(); ++i)
        {
            m_contourVals.push_back(Data::GetInstance()->GetMinValue() + (i * pSettings->GetInterval()));
        }
        GenerateContourColors();
    }
}


// #################### private ####################

// Clears the Renderer.
void Renderer::Clear()
{
    m_contours.clear();
}

// Generates the colors for the contour lines.
void Renderer::GenerateContourColors()
{
    m_contourColors.clear();

    auto pSettings = Settings::GetInstance();

    auto numContours = pSettings->GetNumContours();
    auto pColorScheme = pSettings->GetColorScheme();

    auto colorPositions = pColorScheme->GetPositions();
    auto colors = pColorScheme->GetColors();
    auto contourRatio = 1.0 / (numContours - 1);

    for (auto i = 0; i < numContours; i++)
    {
        auto ratio = (i * contourRatio < 1) ? i * contourRatio : 1;
        auto colorIndex = 0;
        for (int j = 0; j < pColorScheme->GetNumColors(); j++)
        {
            if (ratio <= colorPositions.at(j))
            {
                colorIndex = j;
                break;
            }
        }
        if (colorIndex == 0)
        {
            colorIndex = 1;
        }

        auto color1 = colors.at(colorIndex - 1);
        auto color2 = colors.at(colorIndex);
        auto pos1 = colorPositions.at(colorIndex - 1);
        auto pos2 = colorPositions.at(colorIndex);
        auto relativeRatio = (ratio - pos1) / (pos2 - pos1);

        auto color = InterpolateColor(color1, color2, relativeRatio);

        m_contourColors.push_back(color);
    }
}