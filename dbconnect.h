#ifndef DBCONNECT_H
#define DBCONNECT_H

#include <QObject>
#include <QtSql>
#include <QDebug>
#include <QMessageBox>

class DBConnect : public QObject
{
    Q_OBJECT
public:
    explicit DBConnect(const QString& , const QString&, QObject *parent = 0);
    ~DBConnect();

    QSqlDatabase getDBconnection(){ return m_db; }
    QSqlDatabase m_db;  //represents a connection to a database

private:

    QString m_dbType = "QSQLITE";   //database driver type
    QString m_dirPath = "/Projects/sqlitedb/";  //path to local sqlite database
    QString m_dbName;   //give the database a name
    QString m_connName; //unique connection name

signals:

public slots:

};

#endif // DBCONNECT_H
