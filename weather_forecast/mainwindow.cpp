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

int stop = 0; // индикатор повторного поиска данных по иному сокращению времени

void MainWindow::on_pushButton_clicked()
{
    int k = 0;
    double lat = ui->latitude->text().toDouble();
    double lon = ui->longitude->text().toDouble();
    string day = ui->day->text().toStdString();
    string hour = ui->hour->text().toStdString();
    double rad = ui->radius->text().toDouble();

    int intHour = stoi(hour);
    if (intHour >= 2 && intHour <= 4){
        hour = "03";
    } else if (intHour >= 5 && intHour <= 7){
        hour = "06";
    } else if (intHour >= 8  && intHour <= 10) {
        hour = "09";
    } else if (intHour >= 11  && intHour <= 13) {
        hour = "12";
    }else if (intHour >= 14 && intHour <= 16) {
        hour = "15";
    }else if (intHour >= 17  && intHour <= 19) {
        hour = "18";
    } else if (intHour >= 20) {
        hour = "21";
    }else if (intHour <= 1) {
        hour = "00";
    }
    vector<string> answer = calculateWeather(lat, lon, day, hour, rad);
    QString xAnswer;
    // qDebug() << QString::fromStdString(answer[0]);

    if (stod(answer[0]) == -999.0 && stop == 0)
    {
        if (intHour >= 0 && intHour <= 6){
            hour = "03";
        } else if (intHour >= 7 && intHour <= 12){
            hour = "09";
        } else if (intHour >= 13  && intHour <= 18) {
            hour = "15";
        } else if (intHour >= 19  && intHour <= 23) {
            hour = "21";
        }
        stop++;
        answer = calculateWeather(lat, lon, day, hour, rad);
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
    // qDebug() << QString::number(k);
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

