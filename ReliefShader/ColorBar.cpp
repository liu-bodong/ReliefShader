#include "ColorBar.h"
#include <qpainter.h>
#include "ColorScheme.h"

ColorBar::ColorBar(QWidget* parent)
    : QFrame(parent)
{}

ColorBar::~ColorBar()
{}

void ColorBar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = this->frameRect();
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());

    auto colors = m_colorScheme->GetColors();
    auto pos = m_colorScheme->GetPositions();

    for (auto i = 0; i < colors.size(); ++i)
    {
        QColor color = QColor(std::get<0>(colors[i]), std::get<1>(colors[i]), std::get<2>(colors[i]));
        gradient.setColorAt(1 - pos[i], color);
    }

    painter.fillRect(rect, gradient);
}

//// Draw color bar
//for (int i = 0; i < height(); ++i)
//{
//    painter.setPen(QColor::fromHsvF(i / (double)height(), 1.0, 1.0));
//    painter.drawLine(0, i, width(), i);
//}