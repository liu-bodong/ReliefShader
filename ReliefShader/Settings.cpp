#include "Settings.h"
#include "Data.h"

Settings* Settings::g_pInstance = new (std::nothrow) Settings();

void Settings::SetImageHeight(int imageHeight)
{
    auto pData = Data::GetInstance();
    m_imageHeight = imageHeight;
    m_imageWidth = static_cast<int>(pData->GetAspectRatio() * imageHeight);
    m_resolution = pData->GetOriginalWidth() / m_imageWidth;
}

void Settings::SetImageWidth(int imageWidth)
{
    auto pData = Data::GetInstance();
    m_imageWidth = imageWidth;
    m_imageHeight = static_cast<int>(imageWidth / pData->GetAspectRatio());
    m_resolution = pData->GetOriginalWidth() / m_imageWidth;
}

void Settings::SetResolution(double resolution)
{
    auto pData = Data::GetInstance();
    m_resolution = resolution;
    m_imageWidth = static_cast<int>(resolution * pData->GetOriginalWidth());
    m_imageHeight = static_cast<int>(resolution * pData->GetOriginalHeight());
}

void Settings::SetInterval(double interval)
{
    auto pData = Data::GetInstance();
    m_interval = interval;
    m_numContours = static_cast<int>(pData->GetValueRange() / interval);
}

void Settings::SetNumContours(int numContours)
{
    auto pData = Data::GetInstance();
    m_numContours = numContours;
    m_interval = pData->GetValueRange() / numContours;
}