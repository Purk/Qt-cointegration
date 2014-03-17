#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //MasterSymbolList *masterlist = new MasterSymbolList;
    //  connect signals and slots
    //QObject::connect(masterlist, SIGNAL(accepted()),this, SLOT(on_symbol_list_pushButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_symbol_list_pushButton_clicked()
{
    //MasterSymbolList mlist;
    //launch symbol list window
    m_masterlist = new MasterSymbolList(this);
    m_masterlist->show();
    //masterlist.setModal(true);
    //masterlist.exec();

}

void MainWindow::on_actionGet_Historical_Prices_triggered()
{
    GetPrices* _getprices = new GetPrices(this);
    _getprices->setPrices();

//    QMessageBox msgBox;

//    msgBox.setText("This action will fetch new prices for all securities in your symbol list.");
//    msgBox.setInformativeText("It may take some time. Continue?");
//    msgBox.setIcon(QMessageBox::Question);
//    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
//    msgBox.setDefaultButton(QMessageBox::Cancel);
//    int ret = msgBox.exec();

//    if(ret == QMessageBox::Ok){
//        //m_prices = new GetPrices(this);
//        //m_prices->show();

//        m_prices = new GetPrices(this);
//        m_prices->setPrices();// FetchURL();
//        //delete m_prices;
//    }
}
