#pragma once
#include <vector>
#include <tuple>

class ColorScheme
{
public:
    using RGB = std::tuple<short, short, short>;

    ColorScheme() = default;
    ~ColorScheme() = default;

    std::vector<RGB> GetColors() const { return m_colors; }
    std::vector<double> GetPositions() const { return m_positions; }
    void AddColor(RGB color, double position) { m_colors.push_back(color); m_positions.push_back(position); }
    void AddColor(int r, int g, int b, double position) { m_colors.push_back(RGB(r, g, b)); m_positions.push_back(position); }
    int GetNumColors() const { return int(m_colors.size()); }

private:
    std::vector<RGB> m_colors;
    std::vector<double> m_positions;
};



