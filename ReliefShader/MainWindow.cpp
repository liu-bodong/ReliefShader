#include "MainWindow.h"
#include "Renderer.h"
#include <qpainter.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "ColorScheme.h"
#include <vector>

// ####################### Public #########################
MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    Init();
}

MainWindow::~MainWindow()
{}


// ####################### Slots #########################

void MainWindow::on_loadFileButton_clicked()
{
    ui.statusLabel->setText("Loading File");
    if (LoadFile())
    {
        ui.hLineEdit->setEnabled(true);
        ui.wLineEdit->setEnabled(true);
        ui.resLineEdit->setEnabled(true);
        ui.interLineEdit->setEnabled(true);
        ui.numLineEdit->setEnabled(true);
        ui.saveSettingsButton->setEnabled(true);
        ui.loadSettingsButton->setEnabled(true);
        ui.resetButton->setEnabled(true);
        ui.renderButton->setEnabled(true);

        ui.statusLabel->setText("Loading successful!");
        ui.hLineEdit->setText(QString::number(m_pRenderer->GetRenderedHeight()));
        ui.numLineEdit->setText(QString::number(m_pRenderer->GetNumContours()));
        emit ui.hLineEdit->editingFinished();
        emit ui.numLineEdit->editingFinished();
    }
    else
    {
        ui.statusLabel->setText("Loading failed!");
    }
}

void MainWindow::on_saveSettingsButton_clicked()
{
    ui.statusLabel->setText("Saving Settings");
    if (SaveSettings())
    {
        ui.statusLabel->setText("Saving successful!");
    }
}

void MainWindow::on_loadSettingsButton_clicked()
{
    ui.statusLabel->setText("Loading Settings");
    if (LoadSettings())
    {
        ui.statusLabel->setText("Loading successful!");
    }
}

void MainWindow::on_renderButton_clicked()
{
    ui.statusLabel->setText("Rendering");
    m_pRenderer->Render();
    ui.statusLabel->setText("Rendering successful!");
    ui.exportButton->setEnabled(true);
    ui.previewFrame->update();
}

void MainWindow::on_wLineEdit_editingFinished() {
    m_pRenderer->SetRenderedWidth(ui.wLineEdit->text().toInt());
    m_pRenderer->SyncHeight();
    m_pRenderer->SyncResolution();
    ui.hLineEdit->setText(QString::number(m_pRenderer->GetRenderedHeight()));
    ui.resLineEdit->setText(QString::number(m_pRenderer->GetResolution()));
}

void MainWindow::on_hLineEdit_editingFinished()
{
    m_pRenderer->SetRenderedHeight(ui.hLineEdit->text().toInt());
    m_pRenderer->SyncWidthUsingHeight();
    m_pRenderer->SyncResolution();
    ui.wLineEdit->setText(QString::number(m_pRenderer->GetRenderedWidth()));
    ui.resLineEdit->setText(QString::number(m_pRenderer->GetResolution()));
}

void MainWindow::on_resLineEdit_editingFinished()
{
    m_pRenderer->SetResolution(ui.resLineEdit->text().toDouble());
    m_pRenderer->SyncWidthUsingResolution();
    m_pRenderer->SyncHeight();
    ui.wLineEdit->setText(QString::number(m_pRenderer->GetRenderedWidth()));
    ui.hLineEdit->setText(QString::number(m_pRenderer->GetRenderedHeight()));
}

void MainWindow::on_interLineEdit_editingFinished()
{
    m_pRenderer->SetInterval(ui.interLineEdit->text().toDouble());
    m_pRenderer->SyncNumContours();
    m_pRenderer->SyncContours();
    ui.numLineEdit->setText(QString::number(m_pRenderer->GetNumContours()));
    emit updateContours();
}

void MainWindow::on_numLineEdit_editingFinished()
{
    m_pRenderer->SetNumContours(ui.numLineEdit->text().toInt());
    m_pRenderer->SyncInterval();
    m_pRenderer->SyncContours();
    ui.interLineEdit->setText(QString::number(m_pRenderer->GetInterval()));
    emit updateContours();
}

void MainWindow::on_updateContours()
{
    std::vector<double> contourValues = m_pRenderer->GetStepValues();

    // clear all labels
    QLayoutItem* item;
    while ((item = ui.contourFrame->layout()->takeAt(0)) != nullptr)
    {
        if (item->widget()) {
            item->widget()->setParent(NULL);
        }
        delete item;
    }

    for (int i = m_pRenderer->GetNumContours() - 1; i >= 0; i--)
    {
        auto label = new QLabel(QString::number(contourValues[i]));
        ui.contourFrame->layout()->addWidget(label);
    }

    // create a gradient color from red (top) to green (bottom) on ui.colorFrame
    ui.colorBarFrame->update();
}

void MainWindow::on_gradientCheckBox_stateChanged()
{
    m_pRenderer->SetUseGradient(ui.gradientCheckBox->isChecked());
}

// ####################### Private #########################

bool MainWindow::LoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Obj files (*.obj)"));
    if (fileName.isEmpty())
    {
        return false;
    }
    return m_pRenderer->LoadFile(fileName);
}

bool MainWindow::LoadSettings()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Settings"), "", tr("Text file (*.txt)"));
    if (fileName.isEmpty())
    {
        return false;
    }
    return m_pRenderer->LoadSettings(fileName);
}

bool MainWindow::SaveSettings()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text file (*.txt)"));
    if (fileName.isEmpty())
    {
        return false;
    }
    return m_pRenderer->SaveSettings(fileName);
}

void MainWindow::Init()
{
    m_pRenderer = Renderer::GetInstance();

    QRegExp regExp("0|[1-9]\\d{0,4}");
    ui.hLineEdit->setValidator(new QRegExpValidator(regExp, this));
    ui.wLineEdit->setValidator(new QRegExpValidator(regExp, this));
    ui.resLineEdit->setValidator(new QDoubleValidator(0.0, 100.0, 3, this));
    //ui.interLineEdit->setValidator(new QDoubleValidator(0.0, 100.0, 3, this));
    //ui.numLineEdit->setValidator(new QIntValidator(1, 99, this));

    connect(this, &MainWindow::updateContours, this, &MainWindow::on_updateContours);

    // temporary
    auto defaultColorScheme = new ColorScheme();
    defaultColorScheme->AddColor(162, 215, 90, 0.0);
    defaultColorScheme->AddColor(139, 134, 78, 0.3);
    defaultColorScheme->AddColor(205, 133, 0, 0.5);
    defaultColorScheme->AddColor(165, 42, 42, 0.75);
    defaultColorScheme->AddColor(132, 112, 255, 1.0);





    ui.colorBarFrame->SetColorScheme(defaultColorScheme);

    m_pRenderer->SetColorScheme(defaultColorScheme);
}
