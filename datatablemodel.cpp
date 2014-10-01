#include <QTime>
#include "datatablemodel.h"

DataTableModel::DataTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    //    selectedCell = 0;

}

DataTableModel::DataTableModel(QList<Statistics> allstats, QObject *parent) :
    QAbstractTableModel(parent), _allstats(allstats), _rows(allstats.size())
{
    qDebug() << "dataTM constructor" <<_allstats.size();
//    _timer = new QTimer(this);
//    _timer->setInterval(2000);
//    connect(_timer, SIGNAL(timeout()) , this, SLOT(timerHit()));
//    _timer->start();
}

DataTableModel::~DataTableModel()
{

}

void DataTableModel::setStats(const QList<Statistics> &stats)
{
//    emit dataChanged( QModelIndex(), QModelIndex() );
    beginResetModel();
    _allstats = stats;
    endResetModel();
}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
   return _rows;
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}


QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0:
            return _allstats.at(index.row()).dependent();
        case 1:
            return _allstats.at(index.row()).independent();
        case 2:
            return _allstats.at(index.row()).tstat();
        case 3:
            return _allstats.at(index.row()).zscore();
        case 4:
//            qDebug() << "allstize" << _allstats.size();
            return _allstats.at(index.row()).beta();
        }
    }
    return QVariant();
}
QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            switch(section)
            {
            case 0:
                return QString("dependent");
            case 1:
                return QString("independent");
            case 2:
                return QString("tstat");
            case 3:
                return QString("zscore");
            case 4:
                return QString("beta");
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
{
    return Qt::NoItemFlags | QAbstractTableModel::flags(index);
}

void DataTableModel::timerHit()
{
    qDebug() << "timerHit called";
    //we identify the top left cell
    QModelIndex topLeft = createIndex(0,0);
    //emit a signal to make the view reread identified data
    emit dataChanged(topLeft, topLeft);
}



