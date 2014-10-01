#include "mastersymbollist.h"
#include "ui_mastersymbollist.h"

MasterSymbolList::MasterSymbolList(QWidget *parent) :
    QDialog(parent), ui(new Ui::MasterSymbolList)
{
    ui->setupUi(this);

    msListdB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "masterdB"));
    msListdB->setDatabaseName(QString(QDir::homePath() + "/.sqlitedb/symbol_list.sqlite"));

    if(!msListdB->open())
    {
        qDebug() << "Error opening" << msListdB->databaseName();
    }

    if(msListdB->isValid() && msListdB->isOpen())
    {
        QSqlQuery *query = new QSqlQuery(*msListdB);

        if(!query->exec("CREATE TABLE IF NOT EXISTS [symbol_list] (symbol TEXT, integrated REAL)"))
        {
            QMessageBox::warning(this, tr("Cached table"), tr("dBase query error: %1")
                                 .arg(query->lastQuery()).arg(query->lastError().text()));
        }
        if(!query->exec("CREATE TABLE IF NOT EXISTS [current_pos] (symbol TEXT)"))
        {
            QMessageBox::warning(this, tr("Cached Table"), tr("dBase query error: %1 : %2")
                                 .arg(query->lastQuery()).arg(query->lastError().text()));
        }
        delete query;

    }else{
        qDebug() << "Lost connection to" << msListdB->databaseName();
    }


    model = new QSqlTableModel(this, *msListdB);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("symbol_list");
    model->select();
    model->sort(0, Qt::AscendingOrder);

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);
}

MasterSymbolList::~MasterSymbolList()
{
    delete ui;
}

void MasterSymbolList::on_Update_clicked()
{
    //update
    QString currindx;
    QModelIndex qmi= ui->tableView->currentIndex();
    currindx = qmi.data(Qt::DisplayRole).toString();

    model->database().transaction();
    if(model->submitAll()){
        model->database().commit();
        model->sort(0, Qt::AscendingOrder);       
    }
    else{
        model->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                                     tr("The database (MSL) reported an error: %1")
                                     .arg(model->lastError().text()));
    }
}

void MasterSymbolList::on_Delete_clicked()
{
    QSqlQuery qry1(*msListdB);
    QString currindx;
    QModelIndex qmi= ui->tableView->currentIndex();
    currindx = qmi.data(Qt::DisplayRole).toString();

    if(qry1.exec("SELECT EXISTS (SELECT 1 FROM current_pos WHERE symbol=\'"+currindx+"\' COLLATE NOCASE)")){
        qry1.next();

        if(qry1.value(0).toBool()){
            qDebug() << currindx << "is in your current position and must be removed before being deleted from symbol list";
        }
        else{
            model->database().transaction();
            model->removeRows(ui->tableView->currentIndex().row(),1);
            if(model->submitAll()){
                model->database().commit();
                model->sort(0, Qt::AscendingOrder);
                qDebug() << currindx << "has been deleted from symbol list";
            }
        }
    }
    else{
        qDebug() << qry1.lastQuery();
        qDebug() << "MSL delete error:" <<qry1.lastError();
    }
}

void MasterSymbolList::on_Insert_clicked()
{
    //insert
    model->database().transaction();
    int row = model->rowCount();
    model->insertRows(row,1);
    if(model->submitAll()){
        model->database().commit();
        model->sort(0, Qt::AscendingOrder);
    }
}


void MasterSymbolList::on_calcIntegrated_clicked()
{
//    GetPrices *recalc = new GetPrices(this);
//    recalc->setPrices();
    GetPrices recalc;
    recalc.setPrices(); // calcIntegrated();
//    delete recalc;
}

void MasterSymbolList::on_Delete_All_clicked()
{
    QSqlQuery qry1(*msListdB);

    QMessageBox msgBox;
    msgBox.setWindowTitle("Delete All Symbols");
    msgBox.setText("Continue to delete all symbols from database?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok)
    {
        model->database().transaction();
        int rows = model->rowCount();
        model->removeRows(0, rows);
        if(model->submitAll()){
            model->database().commit();
            model->sort(0, Qt::AscendingOrder);
        }
    }else
    {
        //if (msgBox.exec() == QMessageBox::Cancel) {
        qDebug() << "cancel";
        msgBox.close();
    }



}
