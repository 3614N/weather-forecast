#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDialog>
#include <QPushButton>
#include "libs.h"
#include "calculator.h"
#include "scraper.h"
#include "errors.h"
#include "connectDatabase.h"

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

    double a = ui->latitude->text().toDouble();
    double b = ui->longitude->text().toDouble();
    int answer = mainProcess(a, b);
    QString xAnswer;
    if ((answer != -999.0) && (answer != 999.0))
    {
        xAnswer = "Погода в данной точке:\n" + QString::number(answer) + " градусов цельсия";
    }
    else if (answer == 999.0)
    {
        xAnswer = "Текущий метод обработки:\nИспользовать интерполяцию";
    }
    else if (answer == -999.0)
    {
        xAnswer = "Текущий метод обработки:\nУкрасть у Вадима";
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Answer");
    QLabel *label = new QLabel(xAnswer, dialog);


    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 100);
    dialog->exec();
}


void MainWindow::on_pushButton_2_clicked()
{

    mainScraper();
    check();
    connectDb();
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Status");
    QLabel *label = new QLabel("Обновилось!", dialog);
    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 75);
    dialog->exec();

}

