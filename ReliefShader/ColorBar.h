#pragma once

#include <QFrame>

class ColorScheme;

class ColorBar : public QFrame
{
    Q_OBJECT

public:
    ColorBar(QWidget* parent);
    ~ColorBar();

    void SetColorScheme(ColorScheme* cs) { m_colorScheme = cs; };

private:
    void paintEvent(QPaintEvent* event) override;
    ColorScheme* m_colorScheme;

};
