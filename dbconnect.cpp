#include "dbconnect.h"

DBConnect::DBConnect(const QString& dbName, const QString& connName, QObject *parent) :
    QObject(parent), m_dbName(dbName), m_connName(connName)
{
    #ifdef Q_OS_UNIX
    m_db = QSqlDatabase::addDatabase(m_dbType, m_connName);
    QDir directory(QDir::homePath() + m_dirPath);  //("/home/rodney/Projects/sqlitedb");
    QString temp(QDir::homePath() + m_dirPath);

    if(!directory.exists()){        
        temp = QDir::toNativeSeparators(temp);
        directory.mkpath(temp);
    }

    m_db.setDatabaseName(temp + m_dbName);
    //m_db.setHostName();
    //m_db.setPort();
    #endif

    if(!m_db.open()){
        //  can also use NULL instead of cast QWidget* (parent)
        QMessageBox msgbox;
        msgbox.warning(qobject_cast<QWidget *> (parent), tr("The database reported an error: %1")
                             .arg(m_db.lastError().text()), tr("..."));

        qDebug() << "database last error:" << m_db.lastError();
    }
    else{
        qDebug() << m_db.connectionName() << "is open";
        //qDebug() << qgetenv("HOME");
    }

}

DBConnect::~DBConnect()
{
    //qDebug() << "~DBConnect() has been called on db name:" << this->m_db.databaseName();

    if(this->m_db.isOpen()){
        //qDebug() << "closing database--" << m_db.databaseName();
        m_db.close();
    }

    if(m_db.contains(m_db.connectionName())){
        QString del;
        del = m_db.connectionName();
        //m_db = QSqlDatabase();
        //qDebug() << "removing database connection--" << m_db.connectionName();
        m_db.QSqlDatabase::removeDatabase(del);
    }
}
