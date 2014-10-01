#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QTimer>
#include <QDebug>
#include "statistics.h"

class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DataTableModel(QObject *parent = 0);
    DataTableModel(QList<Statistics>, QObject *parent = 0);
    ~DataTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setStats(const QList<Statistics> &stats);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QTimer *_timer;

private:
    QList<Statistics> _allstats;
    int _rows;
signals:

public slots:
    void timerHit();

};

#endif // DATATABLEMODEL_H
