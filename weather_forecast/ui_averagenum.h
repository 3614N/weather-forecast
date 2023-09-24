/********************************************************************************
** Form generated from reading UI file 'averagenum.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AVERAGENUM_H
#define UI_AVERAGENUM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AverageNum
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLabel *TempValue;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLabel *WetValue;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLabel *PressureValue;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_8;
    QLabel *windDir;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_9;
    QLabel *WindSpeed;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_10;
    QLabel *DewPoint;

    void setupUi(QWidget *AverageNum)
    {
        if (AverageNum->objectName().isEmpty())
            AverageNum->setObjectName("AverageNum");
        AverageNum->resize(520, 334);
        AverageNum->setStyleSheet(QString::fromUtf8("background-color: #FFFFFF;\n"
""));
        gridLayout_2 = new QGridLayout(AverageNum);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(AverageNum);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_2 = new QLabel(AverageNum);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        TempValue = new QLabel(AverageNum);
        TempValue->setObjectName("TempValue");

        horizontalLayout->addWidget(TempValue);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_3 = new QLabel(AverageNum);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        WetValue = new QLabel(AverageNum);
        WetValue->setObjectName("WetValue");

        horizontalLayout_2->addWidget(WetValue);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_4 = new QLabel(AverageNum);
        label_4->setObjectName("label_4");

        horizontalLayout_3->addWidget(label_4);

        PressureValue = new QLabel(AverageNum);
        PressureValue->setObjectName("PressureValue");

        horizontalLayout_3->addWidget(PressureValue);


        verticalLayout->addLayout(horizontalLayout_3);


        horizontalLayout_7->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_8 = new QLabel(AverageNum);
        label_8->setObjectName("label_8");

        horizontalLayout_4->addWidget(label_8);

        windDir = new QLabel(AverageNum);
        windDir->setObjectName("windDir");

        horizontalLayout_4->addWidget(windDir);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_9 = new QLabel(AverageNum);
        label_9->setObjectName("label_9");

        horizontalLayout_5->addWidget(label_9);

        WindSpeed = new QLabel(AverageNum);
        WindSpeed->setObjectName("WindSpeed");

        horizontalLayout_5->addWidget(WindSpeed);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        label_10 = new QLabel(AverageNum);
        label_10->setObjectName("label_10");

        horizontalLayout_6->addWidget(label_10);

        DewPoint = new QLabel(AverageNum);
        DewPoint->setObjectName("DewPoint");

        horizontalLayout_6->addWidget(DewPoint);


        verticalLayout_2->addLayout(horizontalLayout_6);


        horizontalLayout_7->addLayout(verticalLayout_2);


        gridLayout->addLayout(horizontalLayout_7, 1, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(AverageNum);

        QMetaObject::connectSlotsByName(AverageNum);
    } // setupUi

    void retranslateUi(QWidget *AverageNum)
    {
        AverageNum->setWindowTitle(QCoreApplication::translate("AverageNum", "Form", nullptr));
        label->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p align=\"center\"><span style=\" font-size:18pt;\">\320\230\320\275\321\204\320\276\321\200\320\274\320\260\321\206\320\270\321\217 \320\262 \320\264\320\260\320\275\320\275\320\276\320\271 \321\202\320\276\321\207\320\272\320\265:</span></p></body></html>", nullptr));
        label_2->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\242\320\265\320\274\320\277\320\265\321\200\320\260\321\202\321\203\321\200\320\260:</span></p></body></html>", nullptr));
        TempValue->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">0</span></p></body></html>", nullptr));
        label_3->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\222\320\273\320\260\320\266\320\275\320\276\321\201\321\202\321\214:</span></p></body></html>", nullptr));
        WetValue->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">0</span></p></body></html>", nullptr));
        label_4->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\224\320\260\320\262\320\273\320\265\320\275\320\270\320\265 \321\203\321\200. \320\274\320\276\321\200\321\217:</span></p></body></html>", nullptr));
        PressureValue->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">0</span></p></body></html>", nullptr));
        label_8->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\235\320\260\320\277\321\200\320\260\320\262\320\273. \320\262\320\265\321\202\321\200\320\260:</span></p></body></html>", nullptr));
        windDir->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\275/\320\276</span></p></body></html>", nullptr));
        label_9->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\241\320\272\320\276\321\200\320\276\321\201\321\202\321\214 \320\262\320\265\321\202\321\200\320\260:</span></p></body></html>", nullptr));
        WindSpeed->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\275/\320\276</span></p></body></html>", nullptr));
        label_10->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\242\320\276\321\207\320\272\320\260 \321\200\320\276\321\201\321\213:</span></p></body></html>", nullptr));
        DewPoint->setText(QCoreApplication::translate("AverageNum", "<html><head/><body><p><span style=\" font-size:18pt;\">\320\275/\320\276</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AverageNum: public Ui_AverageNum {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AVERAGENUM_H
