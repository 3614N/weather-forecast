#ifndef AVERAGENUM_H
#define AVERAGENUM_H

#include <QDialog>

namespace Ui {
class AverageNum;
}

class AverageNum : public QDialog
{
    Q_OBJECT

public:
    explicit AverageNum(QWidget *parent = nullptr);
    ~AverageNum();

public slots:
    void receiveValues(double tempValue, double wetValue, double pressureValue, std::string windDirection, double windSpeed, double dewPoint);

private:
    Ui::AverageNum *ui;
};

#endif // AVERAGENUM_H
