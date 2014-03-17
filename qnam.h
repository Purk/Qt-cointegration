#ifndef QNAM_H
#define QNAM_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
//#include <QNetworkProxy>
#include <QUrl>
#include <QDebug>
#include <sstream>

class qnam : public QObject
{
    Q_OBJECT
public:
    explicit qnam(const QUrl& url,QObject *parent = 0);
    ~qnam();

    //std::istringstream m_iss;
    //std::istringstream getStream() { return m_iss; }
    QString prices;

signals:

public slots:

    void replyFinished(QNetworkReply* reply);

private:
    //QPointer<QNetworkAccessManager> _qnam;
    QNetworkAccessManager* m_manager;

    QUrl _originalUrl;
    QUrl _urlRedirectedTo;

    void createQNAM();
    QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                         const QUrl& oldRedirectUrl) const;


};

#endif // QNAM_H
