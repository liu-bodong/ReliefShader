#pragma once

#include <QtWidgets/QWidget>
#include "ui_MainWindow.h"

class Renderer;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void updateContours();

public slots:
    void on_loadFileButton_clicked();
    void on_saveSettingsButton_clicked();
    void on_loadSettingsButton_clicked();

    void on_renderButton_clicked();

    void on_wLineEdit_editingFinished();
    void on_hLineEdit_editingFinished();
    void on_resLineEdit_editingFinished();
    void on_interLineEdit_editingFinished();
    void on_numLineEdit_editingFinished();
    void on_gradientCheckBox_stateChanged();

    void on_updateContours();

private:
    Ui::MainWindow ui;

    bool LoadFile();
    bool LoadSettings();
    bool SaveSettings();

    void Init();

    Renderer* m_pRenderer;
};
