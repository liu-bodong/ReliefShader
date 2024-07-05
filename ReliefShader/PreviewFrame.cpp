#include "PreviewFrame.h"
#include "Renderer.h"
#include <qpainter.h>
#include <qimage.h>
#include "Line.h"
#include "Vertex.h"
#include "ColorScheme.h"
#include <qdebug.h>

PreviewFrame::PreviewFrame(QWidget* parent)
    : QFrame(parent)
{
}

PreviewFrame::~PreviewFrame()
{}


void PreviewFrame::paintEvent(QPaintEvent* event)
{
    m_pRenderer = Renderer::GetInstance();
    if (!m_pRenderer)
    {
        return;
    }

    if (m_pRenderer->IsLocked())
    {
        return;
    }


    auto colors = m_pRenderer->GetColorScheme()->GetColors();
    auto colorPositions = m_pRenderer->GetColorScheme()->GetPositions();
    int imgWidth = m_pRenderer->GetRenderedHeight();
    int imgHeight = m_pRenderer->GetRenderedHeight();
    QImage image(imgWidth, imgHeight, QImage::Format_RGB32);
    auto pixmap = QPixmap(imgWidth, imgHeight);
    QPainter painter(this);

    image.fill(Qt::white);

    auto charVertices = m_pRenderer->GetCharVertices();
    auto contourLines = m_pRenderer->GetContourLines();

    // calculate the color for each contour line
    auto contourValues = m_pRenderer->GetStepValues();
    auto contourRatio = 1.0 / (contourLines.size() - 1);
    QList<QColor> contourColors;
    QList<ColorScheme::RGB> testCColors;


    // colors = [green, khaki, brown, red, purple]
    // positions = [0.0, 0.25, 0.5, 0.75, 1.0]
    // position indicates the expected distribution of colors
    if (contourColors.empty())
    {
        for (auto i = 0; i < contourLines.size(); i++)
        {
            auto ratio = (i * contourRatio < 1) ? i * contourRatio : 1;
            auto colorIndex = 0;
            for (int j = 0; j < colorPositions.size(); j++)
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

            auto r = std::get<0>(color1) + (std::get<0>(color2) - std::get<0>(color1)) * relativeRatio;
            auto g = std::get<1>(color1) + (std::get<1>(color2) - std::get<1>(color1)) * relativeRatio;
            auto b = std::get<2>(color1) + (std::get<2>(color2) - std::get<2>(color1)) * relativeRatio;

            contourColors.push_back(QColor(r, g, b));
            testCColors.push_back(ColorScheme::RGB(r, g, b));
        }
    }


    for (auto i = 0; i < imgWidth * imgHeight; i++)
    {
        // 1. map the pixel back to the original data to get its corresponding (x,y) in original data
        // 2. iterate over all contour lines and calculate its closest distance to each contour line
        // 3. compare the distances and get the contour line that is the closest
        // 3. use the distance to calculate the relative color it should get
        // pixel++;
    }

    double scale = 0.4;

    double xOffset = m_pRenderer->GetXOffset();
    double yOffset = m_pRenderer->GetYOffset();

    //paint contour lines
    for (int i = 0; i < contourColors.size(); i++)
    {
        QColor color = contourColors.at(i);
        painter.setPen(QPen(color, 2));

        for (auto lineSeg : contourLines.at(i))
        {
            auto v1 = charVertices.at(lineSeg.first);
            auto v2 = charVertices.at(lineSeg.second);
            auto x1 = (v1->GetX() - xOffset) * scale;
            auto x2 = (v2->GetX() - xOffset) * scale;
            auto y1 = (v1->GetY() - yOffset) * scale;
            auto y2 = (v2->GetY() - yOffset) * scale;
            QPointF p1(x1, y1);
            QPointF p2(x2, y2);
            painter.drawLine(p1, p2);
        }
    }

}