#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "averagenum.h"
#include "ui_averagenum.h"
#include <QDialog>
#include <QPushButton>
#include "libs.h"
#include "calculator.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    int k = 0;
    double a = ui->latitude->text().toDouble();
    double b = ui->longitude->text().toDouble();
    string c = ui->day->text().toStdString();
    string d = ui->hour->text().toStdString();

    vector<double> answer = mainProcess(a, b, c, d);
    QString xAnswer;
    if ((answer[0] != -999.0) && (answer[0] != 999.0))
    {
        k+=1;
    }
    else if (answer[0] == 999.0)
    {
        xAnswer = "Текущий метод обработки:\nВ разработке...";
    }
    else if (answer[0] == -999.0)
    {
        xAnswer = "Текущий метод обработки:\nВ разработке...";
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Answer");
    QLabel *label = new QLabel(xAnswer, dialog);


    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 100);
    if (k == 1)
    {
        double tempValue = answer[0];
        double wetValue = answer[1];
        double pressureValue = answer[2];

        AverageNum averageNumDialog(this);
        connect(this, SIGNAL(sendValues(double, double, double)), &averageNumDialog, SLOT(receiveValues(double, double, double)));
        emit sendValues(tempValue, wetValue, pressureValue);
        averageNumDialog.exec();
    }
    else dialog->exec();
}


void MainWindow::on_pushButton_2_clicked()
{

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Status");
    QLabel *label = new QLabel("В разработке...", dialog);
    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 75);
    dialog->exec();

}

