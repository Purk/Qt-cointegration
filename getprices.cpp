#include "getprices.h"
//#include "ui_getprices.h"

GetPrices::GetPrices(/*QWidget*/ QObject *parent) :
    QObject(parent), _start(252)
{
    _manager = new QNetworkAccessManager(this);
}

GetPrices::~GetPrices()
{
    _manager->deleteLater();

}

 void GetPrices::setPrices()
{
     QSqlDatabase* setPricesdB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "setPricesdB"));
     setPricesdB->setDatabaseName(QString(QDir::homePath() +"/.sqlitedb/prices.sqlite"));

     if(!setPricesdB->open())
     {
         qDebug() << "Error opening" << setPricesdB->databaseName();
     }

     QSqlDatabase* setTickerdB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "setTickerdB"));
     setTickerdB->setDatabaseName(QString(QDir::homePath() + "/.sqlitedb/symbol_list.sqlite"));

     if(!setTickerdB->open())
     {
         qDebug() << "Error opening" << setTickerdB->databaseName();
     }

    //hard coded start date
    QString month = QString::number(00);
    QString day = QString::number(01);
    QString year = QString::number(2005);


    if(setTickerdB->isValid() && setTickerdB->isOpen())
    {
        //query_symbols set to query symbol_list.sqlite dBase. S/b pointer??
        QSqlQuery *query = new QSqlQuery(*setTickerdB);

        //query symbol_list.sqlite dB for all symbols in masterSymbolList
        if(query->exec("SELECT * FROM symbol_list"))
        {
            while(query->next())
            {
                //pass all symbols from symbol_list.sqlite to QStringList
                _symbolList.append(query->value(0).toString());
            }
        }
        else{
            qDebug() << "QSqlQuery last error: " << query->lastError() << endl;
        }
        delete query;
    }
    else{
        qDebug() << "Lost connection to" << setTickerdB->databaseName();
    }

    if(setPricesdB->isValid() && setPricesdB->isOpen())
    {
        //query_prices will query prices.sqlite dBase
        QSqlQuery *query = new QSqlQuery(*setPricesdB);

        //return a list of tables in prices.sqlite dBase
        QStringList _pricesList(setPricesdB->tables());

        std::for_each(_pricesList.begin(), _pricesList.end(), [&] (QString symbol)
        {
            //clear prices.sqlite tables(symbols)
            query->exec("DROP TABLE IF EXISTS "+symbol);
        } );

        //lambda bug in gcc < 4.8. Compiler complains: 'this' not being captured if [].
        std::for_each(_symbolList.begin(), _symbolList.end(), [&] ( QString stock)
        {
            stock.remove("^");
            //query yahoo.com for historical prices. "To date" s/b today by default.
            const QString &url = "http://ichart.finance.yahoo.com/table.csv?s="+stock+"&a="+month+"&b="+day+"&c="+year;
            const QUrl &_originalUrl(url);
            FetchURL(_originalUrl, stock);
        } );
        delete query;

    }else{
        qDebug() << "Lost connection to" << setPricesdB->databaseName();
    }

    setTickerdB->close();
//    delete setTickerdB;
    setPricesdB->close();
//    delete setPricesdB;

    QStringList list = QSqlDatabase::connectionNames();
    for(QString name : list)
    {
        QSqlDatabase::removeDatabase(name);
    }
}

void GetPrices::FetchURL(const QUrl& _originalUrl , QString stock)
{
    QNetworkRequest request(_originalUrl);
    QNetworkReply* reply = _manager->get(request);

    //lambda: handle the finished reply in replyFinished
    connect(reply, &QNetworkReply::finished, [reply, stock, this]()
    {
         this->replyFinished(reply,stock);
    } );
}

void GetPrices::replyFinished(QNetworkReply* reply, QString stock)
{
    /* Reply is finished!
     * We'll ask for the reply about the Redirection attribute */
    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCodeV.toInt();
//    qDebug() << "http status code: " << status;

    if(status != 200)
    {
        QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << "reason: " << stock << reason;

        QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

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
        QSqlDatabase* finishedPricesdB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "finishedPricesdB"));
        finishedPricesdB->setDatabaseName(QString(QDir::homePath() + "/.sqlitedb/prices.sqlite"));

        if(!finishedPricesdB->open())
        {
            qDebug() << "Error opening"<< finishedPricesdB->databaseName();
        }

        if(finishedPricesdB->isValid() && finishedPricesdB->isOpen())
        {
            //query prices.sqlite and insert table(symbol) with historical prices
            QSqlQuery *query = new QSqlQuery(*finishedPricesdB);

            query->exec(QString("CREATE table %1 (ID INTEGER PRIMARY KEY, date TEXT,"
                "open REAL, high REAL, low REAL, close REAL, volume REAL, adj_close REAL)").arg(stock));

            /**** read data from QNetworkReply here ****/
            QTextStream qStream(reply->readAll());
            QString qbyteStr;

            query->setForwardOnly(true); //more memory efficient?

            finishedPricesdB->transaction(); //speeds up db insert
            do {
                qbyteStr = qStream.readLine();
                qbyteStr.remove("-");
                query->exec(QString("INSERT INTO [%1](date,open,high,low,close,volume,adj_close) "
                                 "VALUES (%2)").arg(stock).arg(qbyteStr));

            } while (!qbyteStr.isNull());
            finishedPricesdB->commit();  //speeds up db insert

            delete query;

        }else{
            qDebug() << "Lost connection to" << finishedPricesdB->databaseName();
        }

        finishedPricesdB->close();
        delete finishedPricesdB;
        QSqlDatabase::removeDatabase("finishedPricesdB");

    }else{
        // handle reply errors here
        qDebug() << "replyFinished error:" << reply->errorString();
    }

    progressCounter++;
    emit updateProgressBar(progressCounter, _symbolList.size());

    /* Clean up. */
    reply->close();
    reply->deleteLater();

    /** When all _symbolList has been inserted into prices.sqlite then call
        clacIntegrated to get I(0) or I(1) for each symbol. Hackey but it works **/
    if(progressCounter >= _symbolList.size())
    {
        calcIntegrated();
    }
}

QUrl GetPrices::redirectUrl(const QUrl& possibleRedirectUrl,
                     const QUrl& oldRedirectUrl) const
{
    QUrl redirectUrl;
    /* Check if the URL is empty and
     * that we aren't being fooled into a infinite redirect loop.
     * We could also keep track of how many redirects we have been to */
    if(!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl)
    {
        redirectUrl = possibleRedirectUrl;
    }
    qDebug() << "redirectUrl called";
    return redirectUrl;
}

void GetPrices::updateProgressBar(int insertsCompleted ,int totalInserts)
{
    emit mainProgressBar(insertsCompleted, totalInserts);
}

void GetPrices::calcIntegrated()
{
    QSqlDatabase* pricesdB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "pricesdB"));
    pricesdB->setDatabaseName(QString(QDir::homePath() + "/.sqlitedb/prices.sqlite"));

    if(!pricesdB->open())
    {
        qDebug() << "Error opening212" << pricesdB->databaseName();
    }

    QSqlDatabase* tickerdB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "tickerdB"));
    tickerdB->setDatabaseName(QString(QDir::homePath() + "/.sqlitedb/symbol_list.sqlite"));

    if(!tickerdB->open())
    {
        qDebug() << "Error opening220" << tickerdB->databaseName();
    }

    //map of all symbols and prices:
    QMultiMap<QString, double> pricesMap;
    //list of all symbols:
    QList<QString> qStockList;
    //list of all symbols with integratedness:
    QList<QPair<QString, int> > listPairs;
    //list of prices for call to ADFTest (integratedness):
    QList<double> prices;
    //remove tickers of I(0)
    QList<QString> removeList;
//    QMultiMap<QString, double> integratedPricesMap;

    if(pricesdB->isValid() && pricesdB->isOpen())
    {
        QSqlQuery *query = new QSqlQuery(*pricesdB);

        query->exec("SELECT name FROM sqlite_master WHERE type='table'");
        while(query->next())
        {
            //place all tickers into a QList
            qStockList.append(query->value(0).toString());
        }
        delete query;

    }else{
        qDebug() << "Lost connection to" << pricesdB->databaseName();
    }

    if(pricesdB->isValid() && pricesdB->isOpen())
    {
        QSqlQuery *query = new QSqlQuery(*pricesdB);
//        int _start = 25; //#of trading days: 252 = 1 year
        foreach(QString tableName, qStockList)
        {
            /* retrieve 252 (253) ADJ_CLOSE historical prices from prices.sqlite for each ticker
                in descending order by ID */
            query->exec(QString("SELECT adj_close FROM %1 WHERE ID <= %2 order by ID DESC").arg(tableName).arg(_start));
            query->last();

            // check if ticker has enough historical prices
            if((query->at()+1 >= _start))
            {
                // reposition query to first record
                query->first();
                query->previous();
                while(query->next())
                {
                    /* Insert key(symbol) and values(prices) into QMultiMap. This is the
                       map with all symbols and prices I(0) and I(1) (not yet calculated) */
                    pricesMap.insert(tableName, query->value(0).toDouble());
                }
            }
        }
        delete query;
//        qDebug() << pricesMap;

    }else{
         qDebug() << "Lost connection to" << pricesdB->databaseName();
    }


    /* Call Cointegration::ADFTest to test for I(0), I(1) on all symbols.
        This needs to be re-written with dB insert inside foreach loop and ADFTest
        returning orderInt so no need to call cTest.getIntegrated. Also, listPairs
        s/b a QMap b/c QStr/int has a key/value relationship */
    Cointegration cTest;
    foreach(QString keys, pricesMap.uniqueKeys())
    {
        prices = pricesMap.values(keys);
        /* should ADFTest() return integrated order instead of getIntegrated()?
           orderInt.first = integratedness */
        QPair<int,double> orderInt = cTest.AugmentedDickeyFullerTest(prices, true);

        // listPairs<QString,int> holds all symbols and corresponding Integratedness.
        listPairs.append(qMakePair(keys, orderInt.first));
        if(orderInt.first == 0)
        {
            //List of I(0) symbols to be removed from pricesMap:
            removeList.append(keys);
        }
    }

    integratedPricesMap = pricesMap;

    for(QString qstr : removeList)
    {
        // remove all I(0) symbols and return integratedPricesMap to MainWindow:
        integratedPricesMap.remove(qstr);
    }

    tickerdB->transaction();   //speeds up QSqlDatabase insert
    if(tickerdB->isValid() && tickerdB->isOpen())
    {
        QSqlQuery *query = new QSqlQuery(*tickerdB);
        for(int i = 0; i < listPairs.size(); ++i)
        {
            //query symbol_list.sqlite and update integrated column with I(1) or I(0).
            query->exec(QString("UPDATE symbol_list SET integrated=%1 WHERE symbol='%2'")
                                 .arg(listPairs.at(i).second).arg(listPairs.at(i).first));
        }
        delete query;
    }else{
        qDebug() << "Lost connection to" << tickerdB->databaseName();
    }
    tickerdB->commit();

    tickerdB->close();
    delete tickerdB;
    QSqlDatabase::removeDatabase("tickerdB");

    pricesdB->close();
    delete pricesdB;
    QSqlDatabase::removeDatabase("pricesdB");

    /** Send I(1) tickers with historical prices to MainWindow::updatePricesMap **/
    emit PricesMapSignal(integratedPricesMap);
}

