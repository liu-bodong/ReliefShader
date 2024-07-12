#pragma once

#include <QFrame>
#include <QColor>
#include <vector>
#include <set>
#include "ColorScheme.h"
#include "Line.h"

class Renderer;
class Vertex;

class PreviewFrame : public QFrame
{
    Q_OBJECT

public:
    PreviewFrame(QWidget* parent);
    ~PreviewFrame();

private:
    void paintEvent(QPaintEvent* event) override;

    QColor ConvertRGBToQColor(ColorScheme::RGB rgb) { return QColor(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb)); }

    QImage m_image;

    int m_imgWidth = 0;
    int m_imgHeight = 0;
    std::vector<Vertex*> m_charVertices;
    std::vector< std::set<DirectedLine*> > m_contours;
    std::vector<double> m_contourVals;
    std::vector<ColorScheme::RGB> m_contourColors;
    double m_scale;
    double m_xOffset = 0;
    double m_yOffset = 0;
    double m_resolution = 0.1;

    std::list<ColorScheme::RGB>::const_iterator m_beg;
    std::list<ColorScheme::RGB>::const_iterator m_end;
};