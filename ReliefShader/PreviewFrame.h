#pragma once

#include <QFrame>

class Renderer;

class PreviewFrame : public QFrame
{
    Q_OBJECT

public:
    PreviewFrame(QWidget* parent);
    ~PreviewFrame();

private:
    void paintEvent(QPaintEvent* event) override;

    //QPixmap& paintContours();

    Renderer* m_pRenderer;
};
