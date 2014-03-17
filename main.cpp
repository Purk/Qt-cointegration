#include "mainwindow.h"
#include "cointegration.h"
#include "dbconnect.h"
#include <QApplication>
#include <QtSql>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    QNetworkProxy proxy;
//    proxy.setType(QNetworkProxy::Socks5Proxy);
//    proxy.setHostName("127.0.0.1");
//    proxy.setPort(9150);
//    //proxy.setUser("username");
//    //proxy.setPassword("password");
//    QNetworkProxy::setApplicationProxy(proxy);

    return a.exec();
}
