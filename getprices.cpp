#include "getprices.h"
#include "ui_getprices.h"

GetPrices::GetPrices(QWidget *parent) :
    QDialog(parent), ui(new Ui::GetPrices), _symbolList("")
{
    ui->setupUi(this);
    _manager = new QNetworkAccessManager(this);

    _dataconnect = new DBConnect("symbol_list.sqlite", "first");
    symbolsDB = _dataconnect->getDBconnection();

    _data2connect = new DBConnect("prices.sqlite", "second");
    pricesDB = _data2connect->getDBconnection();
}

GetPrices::~GetPrices()
{
    delete _dataconnect;
    delete _data2connect;
    delete _manager;
    delete ui;
}

 void GetPrices::setPrices()
{
    //hard coded start date
    QString month = QString::number(00);
    QString day = QString::number(01);
    QString year = QString::number(2005);

//    QString sMth = QString::number(strMth-1);
//    QString sDay = QString::number(strDay);
//    QString sYr = QString::number(strYr);
//    QString eMth = QString::number(endMth-1);
//    QString eDay = QString::number(endDay);
//    QString eYr = QString::number(endYr);

    QStringList _symbolList;

    if(symbolsDB.open())
    {
        qDebug() << "opening db... " << symbolsDB.databaseName() ;

        QSqlQuery query_symbols(symbolsDB);

        if(query_symbols.exec("SELECT * FROM symbol_list"))
        {
            while(query_symbols.next())
            {
                //pass all symbols from dBase to QStringList
                _symbolList.append(query_symbols.value(0).toString());
            }
        }
        else{
            qDebug() << "QSqlQuery last error: " << query_symbols.lastError() << endl;
        }
    }
    else{
        qDebug() << "QSqlDatabase last error: " << symbolsDB.lastError() << endl;
    }
    symbolsDB.close();

    if(pricesDB.open()){
        qDebug() << "db2 is opened..." << endl;

        QSqlQuery query_prices(pricesDB);
        QStringList _pricesList(pricesDB.tables());

        std::for_each(_pricesList.begin(), _pricesList.end(), [&] (QString symbol)
        {
            query_prices.exec("DROP TABLE IF EXISTS "+symbol);

        } );

        //lambda bug in gcc < 4.8. Compiler complains: 'this' not being captured if [].
        std::for_each(_symbolList.begin(), _symbolList.end(), [&] ( QString stock)
        {
            stock.remove("^");
            //const QString& url = "http://ichart.yahoo.com/table.csv?s="+stock+"&a="+sMth+"&b="+sDay+"&c="+sYr+"&d="+eMth+"&e="+eDay+"&f="+eYr+"&g=d&ignore=.csv";
            const QString& url = "http://ichart.finance.yahoo.com/table.csv?s="+stock+"&a="+month+"&b="+day+"&c="+year;
            const QUrl& _originalUrl(url);
            FetchURL(_originalUrl, stock);
        } );
    }
    else{
        qDebug() << "Error2 = " << pricesDB.lastError().text() << endl;
    }
}

void GetPrices::FetchURL(const QUrl& _originalUrl , QString stock)
{
    QNetworkRequest request(_originalUrl);
    QNetworkReply *reply =_manager->get(request);

    //handle the finished reply in replyFinished
    connect(reply, &QNetworkReply::finished, [reply, stock, this]()
    {
         this->replyFinished(reply,stock);
    } );
}

void GetPrices::replyFinished(QNetworkReply* reply, QString stock)
{
    /* Reply is finished!
     * We'll ask for the reply about the Redirection attribute
     * http://doc.trolltech.com/qnetworkrequest.html#Attribute-enum */

    QVariant statusCodeV =
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCodeV.toInt();
    qDebug() << "http status code: " << status;

    if(status != 200)
    {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "reason: " << stock << reason;

        QVariant possibleRedirectUrl =
                 reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        //Deduce if the redirection is valid in the redirectUrl function
        _urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(), _urlRedirectedTo);

        //If the URL is not empty, we're being redirected
        if(!_urlRedirectedTo.isEmpty()) {
            //do another request to the redirection url
            this->_manager->get(QNetworkRequest(_urlRedirectedTo));
        }
        else {
            /* We weren't redirected anymore
             * so we arrived to the final destination...
             * ...so this can be cleared. */
            _urlRedirectedTo.clear();
        }
    }

    if (reply->error() == QNetworkReply::NoError)
    {


        QSqlQuery qry(pricesDB);
        //qry.exec("DROP TABLE IF EXISTS "+stock);
        qry.exec(QString("CREATE table %1 (ID INTEGER NOT NULL PRIMARY KEY, date TEXT,"
            "open REAL, high REAL, low REAL, close REAL, volume REAL, adj_close REAL )").arg(stock));

//        qry.prepare("CREATE TABLE ["+stock+"] (ID INTEGER NOT NULL PRIMARY KEY, date TEXT,"
//            "open REAL, high REAL, low REAL, close REAL, volume REAL, adj_close REAL )");
//        qry.addBindValue(stock);
//        qry.exec();

        // read data from QNetworkReply here
        QTextStream* _data = NULL;
        QTextStream qstream(reply->readAll());
        _data = &qstream;
        QString qline;
        qline = _data->readLine();

        pricesDB.transaction(); //speeds up db insert
        while(!qline.isNull())
        {
            qline = _data->readLine();
            qline.remove("-");
            qry.exec("INSERT INTO ["+stock+"](date,open,high,low,close,volume,adj_close) VALUES ("+qline+")");
        }
        pricesDB.commit();  //speeds up db insert
    }
    // Some http error received
    else
    {
        // handle errors here
        qDebug() << "errors?!";
    }

    /* Clean up. */
    qDebug() << "test";
    reply->close();
    reply->deleteLater();
    //emit downloaded();
}

QUrl GetPrices::redirectUrl(const QUrl& possibleRedirectUrl,
                     const QUrl& oldRedirectUrl) const
{
    QUrl redirectUrl;
    /* Check if the URL is empty and
     * that we aren't being fooled into a infinite redirect loop.
     * We could also keep track of how many redirects we have been to
     * and set a limit to it, but we'll leave that to you.
     */
    if(!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl)
    {
        redirectUrl = possibleRedirectUrl;
    }
    qDebug() << "redirectUrl called";
    return redirectUrl;
}
void GetPrices::downloadFinished()
{
        QMessageBox msgBox;

        msgBox.setText("Your Download has Completed");
        msgBox.setInformativeText("It may take some time. Continue?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
}

