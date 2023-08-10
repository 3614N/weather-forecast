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

int stop = 0;

void MainWindow::on_pushButton_clicked()
{
    int k = 0;
    double x = ui->latitude->text().toDouble();
    double y = ui->longitude->text().toDouble();
    string d = ui->day->text().toStdString();
    string t = ui->hour->text().toStdString();

    int hour = stoi(t);
    if (hour >= 2 && hour <= 4){
        t = "03";
    } else if (hour >= 5 && hour <= 7){
        t = "06";
    } else if (hour >= 8  && hour <= 10) {
        t = "09";
    } else if (hour >= 11  && hour <= 13) {
        t = "12";
    }else if (hour >= 14 && hour <= 16) {
        t = "15";
    }else if (hour >= 17  && hour <= 19) {
        t = "18";
    } else if (hour >= 20) {
        t = "21";
    }else if (hour <= 1) {
        t = "00";
    }
    vector<string> answer = mainProcess(x, y, d, t);
    QString xAnswer;
    qDebug() << QString::fromStdString(answer[0]);

    if (stod(answer[0]) == -999.0 && stop == 0)
    {
        if (hour >= 0 && hour <= 6){
            t = "03";
        } else if (hour >= 7 && hour <= 12){
            t = "09";
        } else if (hour >= 13  && hour <= 18) {
            t = "15";
        } else if (hour >= 19  && hour <= 23) {
            t = "21";
        }
        stop++;
        answer = mainProcess(x, y, t, d);
    }
    else if (stod(answer[0]) == -999.0 && stop != 0)
    {
        xAnswer = "Данных нет.";
    }

    else if (stod(answer[0]) == 999.0)
    {
        xAnswer = "Текущий метод обработки:\nИнтерполяция(в разработке...)";
    }

    if (answer.size() != 1)
    {
        k++;
    }
    qDebug() << QString::number(k);
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Answer");
    QLabel *label = new QLabel(xAnswer, dialog);


    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 100);
    if (k > 0)
    {
        double tempValue = stod(answer[0]);
        double wetValue = stod(answer[1]);
        double pressureValue = stod(answer[2]);
        string windDirection = answer[3];
        double windSpeed = stod(answer[4]);
        double dewPoint = stod(answer[5]);
        //for (string a : answer) qDebug() << QString::fromStdString(a);
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

