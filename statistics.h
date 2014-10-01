#ifndef STATISTICS_H
#define STATISTICS_H

#include <QDebug>
#include <QMetaType>
#include <QAbstractTableModel>

class Statistics
{
public:
    Statistics();
    Statistics(const Statistics &other);
    ~Statistics();

    Statistics(const QString &dependent, const QString &independent, const double &beta,
               const double &tstat, double &zscore);

    QString independent() const;
    QString dependent() const;
    double beta() const;
    double tstat() const;
    double zscore() const;

    void set_independent(QString &independent);
    void set_dependent(QString &dependent);
    void set_beta(double &beta);
    void set_tstat(double &tstat);
    void set_zscore(double &zscore);

private:
    QString _independent;
    QString _dependent;
    double _beta;
    double _tstat;
    double _zscore;

signals:

public slots:

};
// to enable the type to be used with QVariant, declare it using this macro:
Q_DECLARE_METATYPE(Statistics)

//QDebug operator<<(QDebug dbg, const Statistics &statistics);

#endif // STATISTICS_H
