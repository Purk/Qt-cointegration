#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mastersymbollist.h"
#include "getprices.h"
#include "datesetter.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QNetworkProxy>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    DateSetter* _datesetter;

private slots:
    void on_symbol_list_pushButton_clicked();

    void on_actionGet_Historical_Prices_triggered();

private:
    Ui::MainWindow* ui;
    MasterSymbolList* m_masterlist;
    GetPrices* m_prices;

};

#endif // MAINWINDOW_H
