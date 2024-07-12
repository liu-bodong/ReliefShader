#include "MainWindow.h"
#include "Renderer.h"
#include <qpainter.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "ColorScheme.h"
#include <vector>
#include <QTime>

#include "Data.h"
#include "Settings.h"

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
        ui.resetButton->setEnabled(true);
        ui.renderButton->setEnabled(true);

        ui.statusLabel->setText("Loading successful!");
        ui.hLineEdit->setText(QString::number(Settings::GetInstance()->GetImageHeight()));
        ui.numLineEdit->setText(QString::number(Settings::GetInstance()->GetNumContours()));
        emit ui.hLineEdit->editingFinished();
        emit ui.numLineEdit->editingFinished();
    }
    else
    {
        ui.statusLabel->setText("Loading failed!");
    }
}

void MainWindow::on_renderButton_clicked()
{
    ui.statusLabel->setText("Rendering");
    QTime timer;
    timer.start();
    Renderer::GetInstance()->Render();
    ui.statusLabel->setText("Rendering successful!");
    ui.exportButton->setEnabled(true);
    ui.previewFrame->update();
    ui.statusLabel->setText(QString::number(timer.elapsed()));
}

void MainWindow::on_wLineEdit_editingFinished()
{
    Renderer::GetInstance()->Lock();
    auto pSettings = Settings::GetInstance();
    pSettings->SetImageWidth(ui.wLineEdit->text().toInt());
    ui.hLineEdit->setText(QString::number(pSettings->GetImageHeight()));
    ui.resLineEdit->setText(QString::number(pSettings->GetResolution()));
}

void MainWindow::on_hLineEdit_editingFinished()
{
    Renderer::GetInstance()->Lock();
    auto pSettings = Settings::GetInstance();
    pSettings->SetImageHeight(ui.hLineEdit->text().toInt());
    ui.wLineEdit->setText(QString::number(pSettings->GetImageWidth()));
    ui.resLineEdit->setText(QString::number(pSettings->GetResolution()));
}

void MainWindow::on_resLineEdit_editingFinished()
{
    Renderer::GetInstance()->Lock();
    auto pSettings = Settings::GetInstance();
    pSettings->SetResolution(ui.resLineEdit->text().toDouble());
    ui.wLineEdit->setText(QString::number(pSettings->GetImageWidth()));
    ui.hLineEdit->setText(QString::number(pSettings->GetImageHeight()));
}

void MainWindow::on_interLineEdit_editingFinished()
{
    auto pRenderer = Renderer::GetInstance();
    pRenderer->Lock();

    auto pSettings = Settings::GetInstance();
    pSettings->SetInterval(ui.interLineEdit->text().toDouble());
    pRenderer->SyncContours();
    ui.numLineEdit->setText(QString::number(pSettings->GetNumContours()));
    emit updateContours();
}

void MainWindow::on_numLineEdit_editingFinished()
{
    auto pRenderer = Renderer::GetInstance();
    pRenderer->Lock();

    auto pSettings = Settings::GetInstance();
    pSettings->SetNumContours(ui.numLineEdit->text().toInt());
    pRenderer->SyncContours();
    ui.interLineEdit->setText(QString::number(pSettings->GetInterval()));
    emit updateContours();
}

void MainWindow::on_updateContours()
{
    auto pRenderer = Renderer::GetInstance();
    std::vector<double> contourValues = pRenderer->GetStepValues();

    // clear all labels
    QLayoutItem* item;
    while ((item = ui.contourFrame->layout()->takeAt(0)) != nullptr)
    {
        if (item->widget()) {
            item->widget()->setParent(nullptr);
        }
        delete item;
    }

    for (int i = Settings::GetInstance()->GetNumContours() - 1; i >= 0; i--)
    {
        auto label = new QLabel(QString::number(contourValues[i]));
        ui.contourFrame->layout()->addWidget(label);
    }

    // create a gradient color from red (top) to green (bottom) on ui.colorFrame
    ui.colorBarFrame->update();
}

void MainWindow::on_gradientCheckBox_stateChanged()
{
    Settings::GetInstance()->SetUseGradient(ui.gradientCheckBox->isChecked());
}

// ####################### Private #########################

bool MainWindow::LoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Obj files (*.obj)"));
    if (fileName.isEmpty())
    {
        return false;
    }
    return Data::GetInstance()->LoadFile(fileName);
}

void MainWindow::Init()
{
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
    defaultColorScheme->AddColor(205, 133, 0, 0.47);
    defaultColorScheme->AddColor(165, 42, 42, 0.73);
    defaultColorScheme->AddColor(132, 112, 255, 1.0);

    ui.colorBarFrame->SetColorScheme(defaultColorScheme);

    Settings::GetInstance()->SetColorScheme(defaultColorScheme);
}
