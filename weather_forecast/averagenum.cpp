#include "averagenum.h"
#include "ui_averagenum.h"

AverageNum::AverageNum(QWidget *parent) : QDialog(parent), ui(new Ui::AverageNum)
{
    ui->setupUi(this);
}

void AverageNum::receiveValues(double tempValue, double wetValue, double pressureValue)
{
    QLabel *tempLabel = ui->TempValue;
    QLabel *wetLabel = ui->WetValue;
    QLabel *pressureLabel = ui->PressureValue;

    tempLabel->setText(QString::number(tempValue));
    wetLabel->setText(QString::number(wetValue));
    pressureLabel->setText(QString::number(pressureValue));

    QFont newFont("Segoe UI", 20);
    tempLabel->setFont(newFont);
    wetLabel->setFont(newFont);
    pressureLabel->setFont(newFont);
}

AverageNum::~AverageNum()
{
    delete ui;
}
