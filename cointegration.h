#ifndef COINTEGRATION_H
#define COINTEGRATION_H

#include <QObject>
#include <vector>
#include <armadillo>

class Cointegration : public QObject
{
    Q_OBJECT
public:
    explicit Cointegration(QObject *parent = 0);
    explicit Cointegration(arma::vec& timeseries, bool drift = 1, QObject *parent = 0);
    explicit Cointegration(arma::vec& y, arma::vec& x, QObject *parent = 0);
    ~Cointegration();

    void AugmentedDickeyFullerTest(arma::vec&, bool);
    int getintegrated() { return m_integrated; }
    void BreuschGodfreyTest(arma::mat&, arma::vec&);
    void CointegrationTest(arma::vec&, arma::vec&);
    double getadftest() { return m_adftest; }
    double getzscore() { return m_zscore; }
    void PositionHedge(arma::vec&, arma::vec&);
    double getbeta() { return m_beta_y; }
    void ErrorCorrectionModel(arma::vec&, arma::vec&, arma::vec&);

private:
    double m_adftest;
    int m_integrated;
    bool m_bgodtest;
    //double m_dfvalue;
    double m_zscore;
    double m_beta_y;
    //double m_ecmvalue;

    //  (A)DF critical values: assumes model with constant, without trend, T=1, and N=250.
    //  critical points (.10, .05, .01) on rows.
    //  From MacKinnon, "Critical Values For CointegrationTest Tests", for any sample
    //  size T, the est. DF critical value = B_inf + B_1 / T + B_2 / T^2
    const double m_dfcritvalues[3][1] = {{-3.03},{-3.31},{-3.86}};

    //  chi square critical points table:
    //  d.f. on rows up to d.f. = 10, critical points (.10, .05, .01) on columns
    const double m_chisqrtable[10][3] = {{2.71, 3.84, 6.63},{4.61, 5.99, 9.21},{6.25, 7.81, 11.30},
                                          {7.78, 9.49, 13.30},{9.24, 11.10, 15.10},{10.60, 12.60, 16.80},
                                          {12.0, 14.10, 18.50},{13.40, 15.50, 20.10},{14.70, 16.90, 21.70},
                                          {16.0, 18.30, 23.20}};

signals:

public slots:

};

#endif // COINTEGRATION_H
