#include "mainwindow.h"
#include "cointegration.h"
#include <QApplication>
#include <QtSql>
#include <QDebug>


int main(int argc, char *argv[])
{

    QApplication application(argc, argv);
    qRegisterMetaType<Statistics>();
    MainWindow w;
    w.show();

    return application.exec();

}
