#include "datesetter.h"
#include "ui_datesetter.h"

DateSetter::DateSetter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateSetter)
{
    ui->setupUi(this);

    DateSetter::setWindowTitle("Set Dates");

    ui->start_dateEdit->setDisplayFormat("MMM-dd-yyyy");
    ui->start_dateEdit->setMinimumDate(QDate::currentDate().addDays(-3653));
    ui->start_dateEdit->setMaximumDate(QDate::currentDate().addDays(0));
    ui->start_dateEdit->setDate(QDate::currentDate().addYears(-1));

    ui->end_dateEdit_2->setDisplayFormat("MMM-dd-yyyy");
    ui->end_dateEdit_2->setMinimumDate(QDate::currentDate().addDays(-3653));
    ui->end_dateEdit_2->setMaximumDate(QDate::currentDate().addDays(0));
    ui->end_dateEdit_2->setDate(QDate::currentDate());
}

DateSetter::~DateSetter()
{
    delete ui;
}

void DateSetter::on_start_dateEdit_userDateChanged()
{
    //set start date
    _startMonth = ui->start_dateEdit->date().month();
    _startDay = ui->start_dateEdit->date().day();
    _startYear = ui->start_dateEdit->date().year();
}

void DateSetter::on_end_dateEdit_2_userDateChanged()
{
    //set end date
    _endMonth = ui->end_dateEdit_2->date().month();
    _endDay = ui->end_dateEdit_2->date().day();
    _endYear = ui->end_dateEdit_2->date().year();
}

void DateSetter::on_prices_pushButton_clicked()
{
//    int _start_month = ui->start_dateEdit->date().month();
//    int _start_day = ui->start_dateEdit->date().day();
//    int _start_year = ui->start_dateEdit->date().year();
//    int _end_month = ui->end_dateEdit_2->date().month();
//    int _end_day = ui->end_dateEdit_2->date().day();
//    int _end_year = ui->end_dateEdit_2->date().year();   

}
