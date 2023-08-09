#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "averagenum.h"
#include "ui_averagenum.h"
#include <QDialog>
#include <QPushButton>
#include "libs.h"
#include "calculator.h"
#include "detector.h"

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

    vector<string> answer = mainProcess(a, b, c, d);
    QString xAnswer;
    if (answer.size() != 1)
    {
        k+=1;
    }
    else if (stod(answer[0]) == 999.0)
    {
        xAnswer = "Текущий метод обработки:\nВ разработке...";
    }
    else if (stod(answer[0]) == -999.0)
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
        double tempValue = stod(answer[0]);
        double wetValue = stod(answer[1]);
        double pressureValue = stod(answer[2]);
        string windDirection = answer[3];
        double windSpeed = stod(answer[4]);
        double dewPoint = stod(answer[5]);
        for (string a : answer) qDebug() << QString::fromStdString(a);
        AverageNum averageNumDialog(this);
        connect(this, SIGNAL(sendValues(double, double, double, std::string, double, double)), &averageNumDialog, SLOT(receiveValues(double, double, double, std::string, double, double)));
        emit sendValues(tempValue, wetValue, pressureValue, windDirection, windSpeed, dewPoint);
        averageNumDialog.exec();
    }
    else dialog->exec();
}


void MainWindow::on_pushButton_2_clicked()
{
    int Q = 0;
    ifstream file("cleared.txt");
    string line;
    getline(file, line);
    if (line.empty())
    {
        vector<string> times;
        for (int i = 0; i < 24; i++)
        {
            if (i < 10) times.push_back("0" + to_string(i));
            else times.push_back(to_string(i));
        }
        for (string time : times) {
            udalyator(time);
            Q++;
        }
    }
    if (Q > 0) {
        ofstream outputFile("cleared.txt");
        outputFile << "1";
        outputFile.close();
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Status");
    QLabel *label = new QLabel("Аномальные значения удалены.", dialog);
    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 75);
    dialog->exec();

}

