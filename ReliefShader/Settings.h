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

class ColorScheme;


class Settings
{
public:
    static Settings* GetInstance()
    {
        return g_pInstance;
    }

    int GetImageHeight() const { return m_imageHeight; }
    int GetImageWidth() const { return m_imageWidth; }
    double GetResolution() const { return m_resolution; }
    double GetInterval() const { return m_interval; }
    int GetNumContours() const { return m_numContours; }
    bool GetUseGradient() const { return m_useGradient; }
    ColorScheme* GetColorScheme() const { return m_pColorScheme; }

    void SetImageHeight(int);
    void SetImageWidth(int);
    void SetResolution(double);
    void SetInterval(double);
    void SetNumContours(int);
    void SetUseGradient(bool useGradient) { m_useGradient = useGradient; }
    void SetColorScheme(ColorScheme* pColorScheme) { m_pColorScheme = pColorScheme; }

private:
    Settings() = default;
    ~Settings() = default;

    static Settings* g_pInstance;

    int m_imageHeight = 1080;
    int m_imageWidth;
    double m_resolution;
    double m_interval;
    int m_numContours = 10;
    bool m_useGradient = false;
    ColorScheme* m_pColorScheme = nullptr;
};

