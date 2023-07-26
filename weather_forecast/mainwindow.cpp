#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDialog>
#include <QPushButton>
#include "libs.h"
#include "calculator.h"
#include "scraper.h"
#include "errors.h"

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
    if (answer == 0)
    {
        xAnswer = "Использовать осреднение";
    }
    else if (answer == 1)
    {
        xAnswer = "Использовать интерполяцию";
    }
    else
    {
        xAnswer = "Украсть у Вадима";
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Answer");
    QLabel *label = new QLabel("Текущий метод обработки:\n" + xAnswer, dialog);


    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 100);
    dialog->exec();
}


void MainWindow::on_pushButton_2_clicked()
{
    check();
    //mainScraper();
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Status");
    QLabel *label = new QLabel("В разработке", dialog);
    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 75);
    dialog->exec();


}

