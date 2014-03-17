#ifndef GETPRICES_H
#define GETPRICES_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
//#include <QNetworkProxy>
//#include <QNetworkSession>
#include <QSql>
#include <QUrl>
#include <QProgressBar>
#include <QDebug>
#include "dbconnect.h"
#include "datesetter.h"
//#include "qnam.h"


namespace Ui {
class GetPrices;
}

class GetPrices : public QDialog
{
    Q_OBJECT

public:
    explicit GetPrices(QWidget *parent = 0);
    ~GetPrices();

    void setPrices();

    DBConnect *_dataconnect;
    DBConnect *_data2connect;
    QSqlDatabase symbolsDB;
    QSqlDatabase pricesDB;
    void replyFinished(QNetworkReply*, QString);

signals:

public slots:

private slots:
    void downloadFinished();

private:
    Ui::GetPrices *ui;
    QNetworkAccessManager* _manager;
    QUrl _originalUrl;
    QUrl _urlRedirectedTo;
    QStringList _symbolList;

    void FetchURL(const QUrl&, QString);
    QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                         const QUrl& oldRedirectUrl) const;
};

#endif // GETPRICES_H
