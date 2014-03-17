#ifndef DATESETTER_H
#define DATESETTER_H

#include <QDialog>
#include <QDateEdit>
#include <QDebug>
#include "getprices.h"

namespace Ui {
class DateSetter;
}

class DateSetter : public QDialog
{
    Q_OBJECT

public:
    explicit DateSetter(QWidget *parent = 0);
    ~DateSetter();

    friend class GetPrices;

    QDate startdate;
    QDate enddate;

    int getStartYear() { return _startYear; }
    int getStartDay() { return _startDay; }
    int getStartMonth() { return _startMonth; }
    int getEndYear() { return _endYear; }
    int getEndDay() { return _endDay; }
    int getEndMonth() { return _endMonth; }

private slots:
    void on_start_dateEdit_userDateChanged();  //(const QDate &date);

    void on_end_dateEdit_2_userDateChanged();  //(const QDate &date);

    void on_prices_pushButton_clicked();

private:
    Ui::DateSetter *ui;

    int _startMonth;
    int _startDay;
    int _startYear;
    int _endMonth;
    int _endDay;
    int _endYear;
};

#endif // DATESETTER_H
