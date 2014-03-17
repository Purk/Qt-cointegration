#include "qnam.h"

qnam::qnam(const QUrl &url, QObject *parent) :
    QObject(parent), _originalUrl(url)
{
   // _qnam = createQNAM();
    createQNAM();
}

qnam::~qnam()
{
    qDebug() << "~qnam deconstructor called";

//    if(_qnam) {
//        _qnam->deleteLater();
//    }
}

void qnam::createQNAM()
{
    m_manager = new QNetworkAccessManager(this);

    /* We'll handle the finished reply in replyFinished */
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    /* Let's just create network request for this predifned URL... */
    QNetworkRequest request(this->_originalUrl);
    /* ...and ask the m_manager to do the request. */
    this->m_manager->get(request);

    //return m_manager;
}

void qnam::replyFinished(QNetworkReply* reply)
{
    /*
     * Reply is finished!
     * We'll ask for the reply about the Redirection attribute
     * http://doc.trolltech.com/qnetworkrequest.html#Attribute-enum
     */

    // Reading attributes of the reply
    // e.g. the HTTP status code
    QVariant statusCodeV =
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCodeV.toInt();
    qDebug() << "http status code: " << status;

    if(status != 200)
    {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "reason: " << reason;

        QVariant possibleRedirectUrl =
                 reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        /* We'll deduce if the redirection is valid in the redirectUrl function */
        _urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),
                                             _urlRedirectedTo);

        /* If the URL is not empty, we're being redirected. */
        if(!_urlRedirectedTo.isEmpty()) {
            /* We'll do another request to the redirection url. */
            this->m_manager->get(QNetworkRequest(_urlRedirectedTo));
        }
        else {
            /*
             * We weren't redirected anymore
             * so we arrived to the final destination...
             */
            /* ...so this can be cleared. */
            _urlRedirectedTo.clear();
        }
    }

    // no error received?
    if (reply->error() == QNetworkReply::NoError)
    {
        // read data from QNetworkReply here
        // Example 2: Reading bytes from the reply
        //QByteArray bytes = reply->readAll();  // bytes
        QString prices = reply->readAll();
        //prices = bytes;
        qDebug() << prices;
//       m_iss(string);
//        m_iss(string.toStdString());
//        std::string str;

//        while(getline(m_iss,str))
//        {
//            QString qstr = QString::fromStdString(str);
//            qstr.remove("-");   //hyphen interpreted as minus sign
//            qDebug() << qstr;
//        }
        //qDebug() << "reply->readAll():" << string;
    }
    // Some http error received
    else
    {
        // handle errors here
        qDebug() << "errors?!";
    }

    /* Clean up. */
    reply->close();
    reply->deleteLater();
}

QUrl qnam::redirectUrl(const QUrl& possibleRedirectUrl,
                               const QUrl& oldRedirectUrl) const
{
    QUrl redirectUrl;
    /*
     * Check if the URL is empty and
     * that we aren't being fooled into a infinite redirect loop.
     * We could also keep track of how many redirects we have been to
     * and set a limit to it, but we'll leave that to you.
     */
    if(!possibleRedirectUrl.isEmpty() &&
       possibleRedirectUrl != oldRedirectUrl)
    {
        redirectUrl = possibleRedirectUrl;
    }
    qDebug() << "redirectUrl called";
    return redirectUrl;
}
