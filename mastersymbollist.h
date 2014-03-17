#ifndef MASTERSYMBOLLIST_H
#define MASTERSYMBOLLIST_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QCloseEvent>
#include "dbconnect.h"

namespace Ui {
class MasterSymbolList;
}

class MasterSymbolList : public QDialog
{
    Q_OBJECT

public:
    explicit MasterSymbolList(QWidget *parent = 0);
    ~MasterSymbolList();

private slots:
    void on_pushButton_clicked();//change

    void on_pushButton_2_clicked();//delete

    void on_pushButton_3_clicked();//add

private:
    Ui::MasterSymbolList *ui;
    QSqlDatabase dblist;
    QSqlTableModel* model;
    DBConnect* mslistdb;
};

#endif // MASTERSYMBOLLIST_H
