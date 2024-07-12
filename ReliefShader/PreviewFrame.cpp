#include "PreviewFrame.h"
#include "Renderer.h"
#include <qpainter.h>
#include <qimage.h>
#include "Line.h"
#include "Vertex.h"
#include "ColorScheme.h"
#include <qdebug.h>
#include <qrgb.h>
#include "Settings.h"
#include "Data.h"
#include "Vector.h"

PreviewFrame::PreviewFrame(QWidget* parent)
    : QFrame(parent)
{
}

PreviewFrame::~PreviewFrame()
{}


void PreviewFrame::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (!Renderer::GetInstance()) { return; }
    if (!Settings::GetInstance()) { return; }
    if (!Data::GetInstance()) { return; }

    auto pRenderer = Renderer::GetInstance();
    auto pSettings = Settings::GetInstance();
    auto pData = Data::GetInstance();

    // �����Ⱦ�����������������ݳ�Ա�ĸ��£��������Ƿ�ֹ�������û���� ��Render�� ��ť֮ǰ�Զ�ˢ����ʾ
    if (!Renderer::GetInstance()->IsLocked())
    {
        m_imgWidth = pSettings->GetImageWidth();
        m_imgHeight = pSettings->GetImageHeight();
        m_contours = pRenderer->GetContourLines();
        m_contourVals = pRenderer->GetStepValues();
        m_contourColors = pRenderer->GetContourColors();
        m_scale = 0.5; // ���ű�������������ǰ��մ��ڴ�С��ͼ���С����ģ������Ϊ0.5���ڲ鿴ϸ��
        m_xOffset = pData->GetMinX();
        m_yOffset = pData->GetMinY();
        m_resolution = pSettings->GetResolution();
        m_image = QImage(m_imgWidth, m_imgHeight, QImage::Format_RGB32);

        auto pPixel = (QRgb*)(m_image.bits()); // pointer to the first pixel in the scanLine

        // ����ÿһ�����ص���ɫ
        // ��Ϊ���ڵĳ���������������ֻ�뿴�ȸ��ߣ�����ע�͵����ѭ��
        for (int i = 0; i < (m_imgWidth * m_imgHeight); i++)
        {
            auto x = (i / m_imgWidth) * m_resolution + m_xOffset;
            auto y = (i % m_imgWidth) * m_resolution + m_yOffset;
            auto color = pRenderer->GetColorAt(x, y);
            *pPixel = qRgb(std::get<0>(color), std::get<1>(color), std::get<2>(color));
            pPixel++;
        }
    }

    // paint the image
    painter.drawImage(0, 0, m_image);

    // paint contour lines
    // ���Ƶȸ��ߣ���������ܸĺô��룬�������ע�͵����߸ĳ�������ɫ�������ɫ����ʾ
    for (int i = 0; i < m_contourColors.size(); i++)
    {
        QColor color = ConvertRGBToQColor(m_contourColors.at(i));
        painter.setPen(QPen(color, 2));

        for (auto line : m_contours.at(i))
        {
            auto v1 = line->a;
            auto v2 = line->b;
            auto x1 = (v1->x - m_xOffset) * m_scale;
            auto x2 = (v2->x - m_xOffset) * m_scale;
            auto y1 = (v1->y - m_yOffset) * m_scale;
            auto y2 = (v2->y - m_yOffset) * m_scale;
            QPointF p1(x1, y1);
            QPointF p2(x2, y2);
            painter.drawLine(p1, p2);
        }
    }
}