#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QNetworkProxy>
#include <QProgressBar>
#include <QtConcurrent>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "mastersymbollist.h"
#include "getprices.h"
#include "cointegration.h"
#include "statistics.h"
#include "datatablemodel.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//    static int const EXIT_CODE_REBOOT = -123456789;
    QProgressBar *_progBar;
    GetPrices *_getprices;
    void downloadFinished();
    void getLastQuote(const QString&);
    QMultiMap<QString, double> mainIntPricesMap;//only I(1) tickers with historical prices

    /* when market is open, insert.at(0) zero to mainIntPricesMap
     * as current price and use replace() to insert last price. */
    QMultiMap<QString, double> mainIntPricesMapOpen;
    void updateLastPrice(const QString &, double &);

signals:
    void emitLastPrice(const QString&, double&);


public slots:


private slots:
    void on_symbol_list_pushButton_clicked();
    void on_actionGet_Historical_Prices_triggered();
    void on_actionStart_Program_triggered();
    void on_Quit_clicked();
    void updateProgressBar(int, int);
    void on_Update_clicked();
    void updatePricesMap(QMultiMap<QString, double>);
    void on_actionQuit_triggered();
    void on_slotReboot_clicked();
//    void updateLastPrice(QString& , double&);

private:
    Ui::MainWindow* ui;
    Cointegration *_statsupdate;
    MasterSymbolList* m_masterlist;
    DataTableModel *_myTableData;
    QNetworkProxy _proxy;
    QTimer *_clockTimer;
    QHash<QString, double> tickerLast;
};

#endif // MAINWINDOW_H
