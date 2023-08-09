#include "averagenum.h"
#include "ui_averagenum.h"

AverageNum::AverageNum(QWidget *parent) : QDialog(parent), ui(new Ui::AverageNum)
{
    ui->setupUi(this);
}

void AverageNum::receiveValues(double tempValue, double wetValue, double pressureValue, std::string windDirection, double windSpeed, double dewPoint)
{
    QLabel *tempLabel = ui->TempValue;
    QLabel *wetLabel = ui->WetValue;
    QLabel *pressureLabel = ui->PressureValue;
    QLabel *windDir = ui->windDir;
    QLabel *WindSpeed = ui->WindSpeed;
    QLabel *dew = ui->DewPoint;

    tempLabel->setText(QString::number(tempValue));
    wetLabel->setText(QString::number(wetValue));
    pressureLabel->setText(QString::number(pressureValue));
    windDir->setText(QString::fromStdString(windDirection));
    WindSpeed->setText(QString::number(windSpeed));
    dew->setText(QString::number(dewPoint));

    QFont newFont("Segoe UI", 20);
    tempLabel->setFont(newFont);
    wetLabel->setFont(newFont);
    pressureLabel->setFont(newFont);
    windDir->setFont(newFont);
    WindSpeed->setFont(newFont);
    dew->setFont(newFont);
}

AverageNum::~AverageNum()
{
    delete ui;
}
