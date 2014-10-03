#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::setWindowTitle("Cointegration Model");

    _clockTimer = new QTimer(this);
    _clockTimer->setInterval(1000);
    connect(_clockTimer, &QTimer::timeout, [=]()
    {
        QTime timenow = QTime::currentTime();
        QString lcdtime = timenow.toString("h : mm : ssa");
        ui->currtimelabel->setText(lcdtime);
    });
    _clockTimer->start();


    _progBar = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(_progBar);
    _progBar->setValue(0);

    ui->tableViewPosition->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /* Set up the trade on QTableWidget */
    ui->tableWidgetStats->setColumnCount(5);
    ui->tableWidgetStats->setRowCount(0);
    QStringList labels;
    labels << tr("dependent") << tr("independent") << tr("tstat") << tr("zscore") << tr("beta");
    ui->tableWidgetStats->setHorizontalHeaderLabels(labels);
    ui->tableWidgetStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /* Set up the ticker, Last Price box */
    ui->tableWidgetTicker->setColumnCount(2);
    ui->tableWidgetTicker->setRowCount(11);
    QStringList tickerLabels;
    tickerLabels << tr("ticker") << tr("Last Price");
    ui->tableWidgetTicker->setHorizontalHeaderLabels(tickerLabels);
    ui->tableWidgetTicker->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //Create the directory home for prices and symbols sqlite databases:
    QDir homedir(QString(QDir::homePath() + "/.sqlitedb"));
    if (!homedir.exists()) {
        homedir.mkpath(".");
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_symbol_list_pushButton_clicked()
{
    //launch symbol list window
    m_masterlist = new MasterSymbolList(this);
    m_masterlist->setAttribute(Qt::WA_DeleteOnClose);
    m_masterlist->show();
}

void MainWindow::updateLastPrice(const QString &_symbol, double &lastPrice)
{
    int rows = ui->tableWidgetTicker->rowCount();
    for(int i = 0; i < rows; i++)
    {
        if(ui->tableWidgetTicker->item(i,0)->text() == _symbol)
        {
            QString lastprc = QString::number(lastPrice);
            ui->tableWidgetTicker->setItem(i,1,new QTableWidgetItem(lastprc));
            qDebug() << "lastPrice =" << lastPrice;
            break;
        }
    }
}

void MainWindow::on_actionGet_Historical_Prices_triggered()
{
    _getprices = new GetPrices(this);
    _getprices->setPrices();

    connect(_getprices,&GetPrices::mainProgressBar,
            this, &MainWindow::updateProgressBar);

    connect(_getprices, &GetPrices::PricesMapSignal,
            this, &MainWindow::updatePricesMap);
}

void MainWindow::updateProgressBar(int insertsRead, int totalInserts)
{
    _progBar->setMaximum(totalInserts);
    _progBar->setValue(insertsRead);

    if(insertsRead >= totalInserts)
    {
        _progBar->reset();
        downloadFinished();
    }
}

void MainWindow::getLastQuote(const QString& _symbol)
{
    QTcpSocket* _tcpMainSocket = new QTcpSocket(this);
    QByteArray byte = _symbol.toUtf8();
    _tcpMainSocket->connectToHost("finance.google.com", 80);

    connect(_tcpMainSocket, &QTcpSocket::connected, [_tcpMainSocket,byte,this]()
    {
        /* Submit HTTP GET request */
         _tcpMainSocket->write("GET /finance/info?client=ig&q="+byte+" \r\n\r\n");//HTTP/1.1
    });

    connect(_tcpMainSocket, &QIODevice::readyRead, [=]()
    {
        //Parse google json file:
        QByteArray qbyte(_tcpMainSocket->readAll());
        _tcpMainSocket->disconnectFromHost();
        int qbytepos = qbyte.indexOf("{");
        qbyte.remove(0, qbytepos - 1);
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(qbyte, &jsonError);
        QJsonObject jsonObject = jsonDoc.object();
        QJsonValue jsonVal = jsonObject.value(QStringLiteral("l_fix"));
        double lastPrice = QString(jsonVal.toString()).toDouble();
        qDebug() << byte << lastPrice;
        mainIntPricesMapOpen.replace(_symbol, lastPrice);
        emit updateLastPrice(_symbol, lastPrice);

    });

    connect(_tcpMainSocket, &QTcpSocket::disconnected, [_tcpMainSocket,this]()
    {
        _tcpMainSocket->deleteLater();
    });
}

void MainWindow::updatePricesMap(QMultiMap<QString, double> intMap)
{
    //Only I(1) tickers with historical prices.
    //Use Map if pre or post open
    mainIntPricesMap = intMap;

    //use MapOpen if market is open
    mainIntPricesMapOpen = intMap;

    for(QString key : mainIntPricesMapOpen.uniqueKeys())
    {
        mainIntPricesMapOpen.insert(key, 0);
    }

    QStringList tickers = mainIntPricesMapOpen.uniqueKeys();
    int rows = (tickers.size() * tickers.size()) - tickers.size();

    /* Set items(ticker symbols) for tableWidgetTicker */
    int t = 0;
    ui->tableWidgetTicker->setRowCount(tickers.size());
    std::for_each(tickers.begin(), tickers.end(), [&](QString str) {
        ui->tableWidgetTicker->setItem(t, 0, new QTableWidgetItem(str));
        t++;
    });

    /* populate the QTableWidget with stock pairs */
    ui->tableWidgetStats->setRowCount(rows);
}

void MainWindow::on_actionStart_Program_triggered()
{
    QTime currTime(QTime::currentTime());
    QTime begin(8,30,0);
    QTime end(15,0,0);
    QList<QString> mapKeys = mainIntPricesMap.uniqueKeys();
    QList<QPair<QString,QString> > pairsList;
    QList<Statistics> newStats;

    if(!mapKeys.isEmpty())
    {
        /* Pair all the I(1) stocks */
        for(QString symbol : mapKeys)
        {
            for(QString nextSymbol : mapKeys)
            {
                if(symbol != nextSymbol)
                {
                    pairsList.append(qMakePair(symbol,nextSymbol));
                }
            }
        }
    }

    if(!mapKeys.isEmpty())
    {
        for(int i = 0; i < pairsList.size(); i++)
        {
            QString yticker = QString(pairsList[i].first);
            QString xticker = QString(pairsList[i].second);
            QList<double> y = mainIntPricesMap.values(yticker);
            QList<double> x = mainIntPricesMap.values(xticker);
            Cointegration cointObject;
            Statistics stats = cointObject.CointegrationTest(yticker, xticker, y, x);
            newStats.append(stats);
        }
    }

    _myTableData = new DataTableModel(newStats);
    ui->tableViewPosition->setModel(_myTableData);

    if(mapKeys.isEmpty())
    {
        QMessageBox::warning(this, tr("Symbol List"),
                             tr("Missing prices for your symbol list. Click 'Get Prices'"));
    }else{
        QTimer *timer = new QTimer(this);

        if(currTime > begin)// && currTime < end)
        {
            connect(timer, &QTimer::timeout, [=](){
                for(QString ticker : mapKeys)
                {
                    /* insert current price into mainIntPricesMapOpen for each ticker */
                    getLastQuote(ticker);
                }

                QList<Statistics> updateStatsOpen;
                for(int i = 0; i < pairsList.size(); i++)
                {
                    QString yticker = QString(pairsList[i].first);
                    QString xticker = QString(pairsList[i].second);
                    QList<double> y = mainIntPricesMapOpen.values(yticker);
                    QList<double> x = mainIntPricesMapOpen.values(xticker);
                    Cointegration cointTestObject;
                    Statistics statsOpen = cointTestObject.CointegrationTest(yticker, xticker, y, x);
                    updateStatsOpen.append(statsOpen);

                }
                _myTableData->setStats(updateStatsOpen);

            });
            timer->start(2000);
        }else{
//            [this] capture the 'this' pointer of the enclosing class
//            connect(timer, &QTimer::timeout, [this, pairsList]() {
                QList<Statistics> updateStats;
                for(int i = 0; i < pairsList.size(); i++)
                {
                    QString yticker = QString(pairsList[i].first);
                    QString xticker = QString(pairsList[i].second);
                    QList<double> y = mainIntPricesMap.values(yticker);
                    QList<double> x = mainIntPricesMap.values(xticker);
                    Cointegration cointTestObject;
                    Statistics stats = cointTestObject.CointegrationTest(yticker, xticker, y, x);
                    updateStats.append(stats);
                }
                _myTableData->setStats(updateStats);
//            });
                /* since view does not own the model it will be destroyed when goes out
                of scope if created on stack. */
//            timer->start(5000);
        }
    }
}

void MainWindow::downloadFinished()
{
    QMessageBox msgBox;

    msgBox.setText("Your Download has Completed");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_Quit_clicked()
{
    qApp->quit();

}

void MainWindow::on_Update_clicked()
{
    QSqlDatabase* updatedB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","updatedB"));
    updatedB->setDatabaseName(QString(QDir::homePath() + "/.sqlitedb/symbol_list.sqlite"));

    if(!updatedB->open())
    {
        qDebug() << "Error opening" << updatedB->databaseName();
    }

    if(updatedB->isValid() && updatedB->isOpen())
    {
        QSqlQueryModel *model = new QSqlQueryModel(this);
        QSqlQuery *query = new QSqlQuery(*updatedB);
        query->prepare("SELECT * FROM symbol_list");
        query->exec();
        model->setQuery(*query);
        ui->integratedStocks->setModel(model);
//        ui->integratedStocks->sortByColumn(0);
        delete query;
    }
}

void MainWindow::on_actionQuit_triggered()
{
    on_Quit_clicked();
}

void MainWindow::on_slotReboot_clicked()
{
    qDebug() << "Performing application reboot...";
    QProcess proc;
    proc.start(qApp->applicationFilePath());

    ::exit(0);
}
