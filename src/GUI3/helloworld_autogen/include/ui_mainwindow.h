/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtCharts>
#include "qchartview.h"
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *cameraLabel;
    QHBoxLayout *infoLayout;
    QGroupBox *motionGroup;
    QVBoxLayout *motionLayout;
    QLabel *motionStatusLabel;
    QGroupBox *thGroup;
    QVBoxLayout *thLayout;
    QLabel *tempLabel;
    QLabel *humLabel;
    QTabWidget *historyTabs;
    QWidget *tempHumPage;
    QVBoxLayout *thChartLayout;
    QtCharts::QChartView *thChartView;
    QChartView *graphicsView;
    QWidget *motionPage;
    QVBoxLayout *motionChartLayout;
    QtCharts::QChartView *motionChartView;
    QChartView *graphicsView_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(658, 784);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cameraLabel = new QLabel(centralwidget);
        cameraLabel->setObjectName(QString::fromUtf8("cameraLabel"));
        cameraLabel->setMinimumSize(QSize(640, 480));
        cameraLabel->setFrameShape(QFrame::Box);

        verticalLayout->addWidget(cameraLabel);

        infoLayout = new QHBoxLayout();
        infoLayout->setObjectName(QString::fromUtf8("infoLayout"));
        motionGroup = new QGroupBox(centralwidget);
        motionGroup->setObjectName(QString::fromUtf8("motionGroup"));
        motionLayout = new QVBoxLayout(motionGroup);
        motionLayout->setObjectName(QString::fromUtf8("motionLayout"));
        motionStatusLabel = new QLabel(motionGroup);
        motionStatusLabel->setObjectName(QString::fromUtf8("motionStatusLabel"));

        motionLayout->addWidget(motionStatusLabel);


        infoLayout->addWidget(motionGroup);

        thGroup = new QGroupBox(centralwidget);
        thGroup->setObjectName(QString::fromUtf8("thGroup"));
        thLayout = new QVBoxLayout(thGroup);
        thLayout->setObjectName(QString::fromUtf8("thLayout"));
        tempLabel = new QLabel(thGroup);
        tempLabel->setObjectName(QString::fromUtf8("tempLabel"));

        thLayout->addWidget(tempLabel);

        humLabel = new QLabel(thGroup);
        humLabel->setObjectName(QString::fromUtf8("humLabel"));

        thLayout->addWidget(humLabel);


        infoLayout->addWidget(thGroup);


        verticalLayout->addLayout(infoLayout);

        historyTabs = new QTabWidget(centralwidget);
        historyTabs->setObjectName(QString::fromUtf8("historyTabs"));
        tempHumPage = new QWidget();
        tempHumPage->setObjectName(QString::fromUtf8("tempHumPage"));
        thChartLayout = new QVBoxLayout(tempHumPage);
        thChartLayout->setObjectName(QString::fromUtf8("thChartLayout"));
        thChartView = new QtCharts::QChartView(tempHumPage);
        thChartView->setObjectName(QString::fromUtf8("thChartView"));

        thChartLayout->addWidget(thChartView);

        graphicsView = new QChartView(tempHumPage);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        thChartLayout->addWidget(graphicsView);

        historyTabs->addTab(tempHumPage, QString());
        motionPage = new QWidget();
        motionPage->setObjectName(QString::fromUtf8("motionPage"));
        motionChartLayout = new QVBoxLayout(motionPage);
        motionChartLayout->setObjectName(QString::fromUtf8("motionChartLayout"));
        motionChartView = new QtCharts::QChartView(motionPage);
        motionChartView->setObjectName(QString::fromUtf8("motionChartView"));

        motionChartLayout->addWidget(motionChartView);

        graphicsView_2 = new QChartView(motionPage);
        graphicsView_2->setObjectName(QString::fromUtf8("graphicsView_2"));

        motionChartLayout->addWidget(graphicsView_2);

        historyTabs->addTab(motionPage, QString());

        verticalLayout->addWidget(historyTabs);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        historyTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Baby Monitor", nullptr));
        motionGroup->setTitle(QCoreApplication::translate("MainWindow", "motion status", nullptr));
        motionStatusLabel->setText(QCoreApplication::translate("MainWindow", "unknown", nullptr));
        thGroup->setTitle(QCoreApplication::translate("MainWindow", "temperature / humidity ", nullptr));
        tempLabel->setText(QCoreApplication::translate("MainWindow", "temperature: -- \302\260C", nullptr));
        humLabel->setText(QCoreApplication::translate("MainWindow", "humidity: -- %", nullptr));
        historyTabs->setTabText(historyTabs->indexOf(tempHumPage), QCoreApplication::translate("MainWindow", "history temp and humidity", nullptr));
        historyTabs->setTabText(historyTabs->indexOf(motionPage), QCoreApplication::translate("MainWindow", "motion history", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
