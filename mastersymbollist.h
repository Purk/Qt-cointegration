#ifndef MASTERSYMBOLLIST_H
#define MASTERSYMBOLLIST_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QCloseEvent>
//#include "dbconnect.h"
//#include "integrated.h"
#include "getprices.h"

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
    void on_Update_clicked();//update

    void on_Delete_clicked();//delete

    void on_Insert_clicked();//insert

    void on_calcIntegrated_clicked();

    void on_Delete_All_clicked();

private:
    Ui::MasterSymbolList *ui;
    QSqlDatabase *msListdB;
    QSqlTableModel* model;
//    DBConnect* mslistdb;
};

#endif // MASTERSYMBOLLIST_H
