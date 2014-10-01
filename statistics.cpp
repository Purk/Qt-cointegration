#include "statistics.h"

Statistics::Statistics()
{
}

Statistics::Statistics(const Statistics &other)
{
    _independent = other._independent;
    _dependent = other._dependent;
    _beta = other._beta;
    _tstat = other._tstat;
    _zscore = other._zscore;
}

Statistics::~Statistics()
{

}

Statistics::Statistics(const QString &dependent, const QString &independent, const double &beta,
                       const double &tstat, double &zscore)
{
    _dependent = dependent;
    _independent = independent;
    _beta = beta;
    _tstat = tstat;
    _zscore = zscore;
//    qDebug() << "beta" << beta;
//    qDebug() << "tstat" << tstat;
//    qDebug() << "zscore" << zscore;
}

//QDebug operator<<(QDebug dbg, const Statistics &statistics)
//{

//}

QString Statistics::independent() const
{
    return _independent;
}

QString Statistics::dependent() const
{
    return _dependent;
}

double Statistics::beta() const
{
    return _beta;
}

double Statistics::tstat() const
{
    return _tstat;
}

double Statistics::zscore() const
{
    return _zscore;
}

void Statistics::set_independent(QString &independent)
{
    _independent = independent;
}

void Statistics::set_dependent(QString &dependent)
{
    _dependent = dependent;
}

void Statistics::set_beta(double &beta)
{
    _beta = beta;
}

void Statistics::set_tstat(double &tstat)
{
    _tstat = tstat;
}

void Statistics::set_zscore(double &zscore)
{
    _zscore = zscore;
}
