#ifndef GETPRICES_H
#define GETPRICES_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
//#include <QNetworkProxy>
//#include <QNetworkSession>
#include <QProgressDialog>
#include <QtSql>
#include <QUrl>
#include <QDebug>
//#include "dbconnect.h"
#include "datesetter.h"
#include "cointegration.h"

//namespace Ui {
//class GetPrices;
//}

class GetPrices : public QObject    //QDialog
{
    Q_OBJECT

public:
    explicit GetPrices(/*QWidget*/ QObject *parent = 0);
    ~GetPrices();

    void setPrices();

//    DBConnect *_dataconnect;  //connection to symbol_list dB
//    DBConnect *_data2connect;  //connection to prices dB
//    QSqlDatabase symbolsDB;
//    QSqlDatabase pricesDB;
    QStringList _symbolList;
    QMultiMap<QString, double> integratedPricesMap;
    void replyFinished(QNetworkReply*, QString);
    int progressCounter = 0;  //update for the qprogressbar

    void calcIntegrated();

signals:
    void mainProgressBar(int, int);
//    void insertProgress(qint64, qint64);
    void PricesMapSignal(QMultiMap<QString, double>);

public slots:
    void updateProgressBar(int, int);

private slots:

private:
//    Ui::GetPrices *ui;
    QNetworkAccessManager* _manager;
    QUrl _originalUrl;
    QUrl _urlRedirectedTo;

    void FetchURL(const QUrl&, QString);
    QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                         const QUrl& oldRedirectUrl) const;
    int _start; //number of trading days.

};

#endif // GETPRICES_H
