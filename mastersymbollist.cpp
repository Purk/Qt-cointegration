#include "mastersymbollist.h"
#include "ui_mastersymbollist.h"

MasterSymbolList::MasterSymbolList(QWidget *parent) :
    QDialog(parent), ui(new Ui::MasterSymbolList)
{
    ui->setupUi(this);


    mslistdb = new DBConnect("symbol_list.sqlite", "test");
    dblist = mslistdb->getDBconnection();


    if(dblist.open())
    {
        QSqlQuery qry(dblist);
        if(qry.exec("CREATE TABLE IF NOT EXISTS [symbol_list] (symbol TEXT)")){
            qry.exec("CREATE TABLE IF NOT EXISTS [current_pos] (symbol TEXT)");
        }
        else{
            qDebug() << qry.lastQuery();
            qDebug() << qry.lastError();
        }
    }
    else{
        qDebug() << dblist.lastError();
    }

    model = new QSqlTableModel(this, dblist);
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
    dblist.close();
    dblist.removeDatabase("test");
    delete mslistdb;
    delete model;
    delete ui;
}

void MasterSymbolList::on_pushButton_clicked()
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
                                     tr("The database reported an error: %1")
                                     .arg(model->lastError().text()));
    }

}

void MasterSymbolList::on_pushButton_2_clicked()
{
    QSqlQuery qry1(dblist);
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
        qDebug() << qry1.lastError();
    }
}

void MasterSymbolList::on_pushButton_3_clicked()
{
    //add
    model->database().transaction();
    int row = model->rowCount();
    model->insertRows(row,1);
    if(model->submitAll()){
        model->database().commit();
        model->sort(0, Qt::AscendingOrder);
    }
}

