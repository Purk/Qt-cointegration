#include "cointegration.h"

Cointegration::Cointegration(QObject *parent) :
    QObject(parent), m_adftest(0), m_integrated(0), m_bgodtest(0),
    m_zscore(0), m_beta_y(0) //, m_ecmvalue(0)
{
}

Cointegration::Cointegration(QList<double> &priceList, bool drift, QObject *parent) :
    QObject(parent), m_adftest(0), m_integrated(0), m_bgodtest(0),
    m_zscore(0), m_beta_y(0) //, m_ecmvalue(0)
{
    arma::vec timeseries(priceList.size());

    for(int i = 0; i < priceList.size(); i++)
    {
        timeseries(i) = priceList.at(i);
    }
}

Cointegration::Cointegration(QList<double> &dependent, QList<double> &independent, QObject *parent) :
    QObject(parent), m_adftest(0), m_integrated(0), m_bgodtest(0),
    m_zscore(0), m_beta_y(0) //, m_ecmvalue(0)
{
    arma::vec yVar(dependent.size());
    arma::vec xVar(independent.size());

    for(int i = 0; i < independent.size(); i++)
    {
        yVar(i) = dependent.at(i);
        xVar(i) = independent.at(i);
    }
}

Cointegration::~Cointegration()
{

}

QPair<int, double> Cointegration::AugmentedDickeyFullerTest(QList<double> &priceList, bool drift)
{
    QPair<int, double> ADFStats;
    arma::vec timeseries(priceList.size());

    for(int i=0; i<priceList.size(); i++)
    {
        timeseries(i) = priceList.at(i);
    }

    int adfIntegrated = 0;
    double variance;
    double tstat;
    bool adfBGTest;
    int rowsize;
    int columnsize;
    int lags = 0;
    arma::vec dependent;
    arma::vec coefficients;
    arma::vec residuals;
    arma::vec standard_error_of_estimate;
    arma::mat independent;

    do{
        rowsize = timeseries.n_rows - 1 - lags;
        columnsize = drift + 1 + lags;  //const+var+lags
        dependent.set_size(rowsize);   //must resize on each pass
        independent.set_size(rowsize, columnsize);  //must resize on each pass

        if(drift == true){
            for(int i = 0; i < rowsize; i++){
                independent(i,0) = 1;   //adds a constant to design matrix
            }
        }

        for(int i = 0; i < rowsize; i++){
            dependent(i) = timeseries(i + 1 + lags) - timeseries(i + lags);   //set up dependent var
             independent(i,drift) = timeseries(i + lags);    //adds independent var to design matrix
        }

        if(lags>0){
            for(int i = 0; i < lags; i++){      //add lags of dependent
                for(int k = 0; k < rowsize; k++){
                    independent(k,2 + i) = timeseries(i + k + 1) - timeseries(i + k);
                }
            }
        }

        //  OLS regression:
        coefficients = arma::inv(arma::trans(independent) * (independent)) * arma::trans(independent) * (dependent);
        residuals = dependent - (independent) * (coefficients);

        //  call BGtest and set m_bgodtest
        bool adfBGTest = BreuschGodfreyTest(independent, residuals);

        if(!adfBGTest){
            lags = lags + 1;
        }

    }while(!adfBGTest && lags < 0);    //arbitrarily chose up to 3 lags

/*-------------tstat----------------------------------
  Variance = sum(residual-mean)^2 / n-k.
  If drift is included in OLS then mean will be 0. Otherwise mean != 0.
  variance = arma::var(residuals);
*/

    variance = arma::as_scalar((arma::trans(residuals) * residuals) / (residuals.n_rows - independent.n_cols));
    standard_error_of_estimate = arma::sqrt(variance * arma::diagvec(arma::inv(arma::trans(independent) * independent)));

    tstat = coefficients(drift) / standard_error_of_estimate(drift);

//    qDebug() << "tsat =" << tstat;

    if (tstat > m_dfcritvalues[1][0]){
        adfIntegrated = 1;
    }
    ADFStats.first = (adfIntegrated);
    ADFStats.second = (tstat);
//    qDebug() << "Coint_120_adfInt =" << ADFStats.first << "tstat =" << ADFStats.second;
    return ADFStats;
}

bool Cointegration::BreuschGodfreyTest(arma::mat& independent, arma::vec& residuals){
    /*The null hypothesis is no serial correlation of any order up to p.
    acLags set to 1 assumes residuals follow an AR(1) process.*/

    bool bgodtest;
    int numrows = residuals.n_rows, numcols = independent.n_cols, acLags = 1;
    double sumSqrs;
    double ySumofSqrs;
    double TSS;
    double RSS;
    double R2;
    double LMtest;
    arma::vec residCoefs;
    arma::vec residErrors;
    arma::vec regressand = arma::zeros(numrows);
    arma::mat regressor = arma::zeros(numrows, numcols + acLags);

    //OLS auxiliary regression dependent var:
    for(int i = 0; i < numrows; i++){
        regressand(i) = residuals(i);
    }

    //OLS auxiliary regression independent matrix vars:
    for(int col = 0; col < numcols; col++){
        for(int i = 0; i < numrows; i++){
            regressor(i,col) = independent(i,col);
        }
    }

    //add lags (acLags) of dependent var to independent matrix vars:
    for(int col = numcols, p = 1; col < numcols + acLags; col++, p++){
        for(int i = 0; i < numrows - p; i++){
            regressor(i + p,col) = residuals(i);
        }
    }

    //OLS auxiliary regression:
    residCoefs = arma::inv(arma::trans(regressor) * regressor) * arma::trans(regressor) * regressand;
    residErrors = regressand - (regressor * residCoefs);

    //arma::mat JMat = arma::ones<arma::mat>(numrows,numrows);

    sumSqrs = arma::sum(square(regressand));
    ySumofSqrs = sum(regressand) * sum(regressand);
    TSS = sumSqrs - (ySumofSqrs / numrows);

    RSS = as_scalar(arma::trans(residErrors) * residErrors);
    //double ESS = as_scalar(trans(yhat) * yhat - (1/ye.n_rows) * trans(ye) * JMat * ye);
    R2 = (TSS - RSS) / TSS;
    LMtest = R2 * numrows; //-acLags;

    //  if LMtest stat is > X^2 critical value then reject null of no autocorrelation.
    if (LMtest > m_chisqrtable[acLags-1][1]){
        bgodtest = false;
    }
    else{
        bgodtest = true;
    }
    return bgodtest;
}

Statistics Cointegration::CointegrationTest(QString &dep, QString &indep,
                                      QList<double> &yVector, QList<double> &xVector)
{
    Statistics theStats;
    arma::vec dependent(yVector.size());
    arma::vec independent(xVector.size());

    for(int i = 0; i < yVector.size(); i++)
    {
        dependent(i) = yVector.at(i);
        independent(i) = xVector.at(i);
    }

    int rowsize = dependent.n_rows;
    arma::mat xmatrix = arma::ones(rowsize,1);   //adds constant to design matrix
    xmatrix.insert_cols(1,independent);   //adds independent variable to design matrix
    arma::vec coefficients(xmatrix.n_cols);
    arma::vec residuals(dependent.n_rows);
    double zscore;
    double tstat;
    double stnd_deviation;

    //  OLS regression:
    coefficients = arma::inv(arma::trans(xmatrix) * (xmatrix)) * arma::trans(xmatrix) * (dependent);
    residuals = dependent - (xmatrix * coefficients);

    //  sc: true = accept null of no autocorrelation; false = reject null and accept alt. of autocorrelation
//    bool BGTest = BreuschGodfreyTest(xmatrix, residuals);

    //  call ADFTest without drift for cointegration test
    QList<double> qListResids;
    for(auto i : residuals)
    {
        qListResids.append(i);
    }

    //ADFstats.first = integratedness, ADFstats.second = tstat
    QPair<int, double> ADFstats = AugmentedDickeyFullerTest(qListResids,false);
    tstat = ADFstats.second;

//    ErrorCorrectionModel(dependent, independent, residuals);

//    if(tstat < m_dfcritvalues[1][0])
//    {
//        //double stnd_deviation = std::sqrt(arma::as_scalar((arma::trans(residuals) * residuals) / (independent.n_rows - independent.n_cols)));

//    }
//    else{
////        qDebug() << "not cointegrated";
//    }

    stnd_deviation = arma::stddev(residuals);
    zscore = residuals(0) / stnd_deviation;
    double beta = PositionHedge(dependent, independent);
    Statistics statObj(dep,indep,beta,tstat,zscore);
//    qDebug() << "beta:" << statObj.beta();
    return statObj;
//    emit sendStats(statObj);
}

void Cointegration::ErrorCorrectionModel(arma::vec& y, arma::vec& x, arma::vec& z)
{
    int numrows = x.n_rows;
    int lags = 1;
    arma::vec xDiff = arma::zeros(numrows-1-lags);
    arma::vec yDiff = arma::zeros(numrows-1-lags);
    arma::mat xMatrix = arma::ones(numrows-1-lags, 4);
    arma::vec x_coefficients;
    arma::vec x_errors;
    arma::vec y_coefficients;
    arma::vec y_errors;
    //int numcols = xMatrix.n_cols;


    //  dependent variable:
    for(int i = 0; i < numrows - 1 - lags; i++){
        yDiff(i) = y(i+1+lags) - y(i+lags);
        xDiff(i) = x(i+1+lags) - x(i+lags);
    }

    //  independent variable matrix:
    for(int i = 0; i < numrows - 1 - lags; i++){
        xMatrix(i,1) = x(i+1) - x(i);
        xMatrix(i,2) = y(i+1) - y(i);
        xMatrix(i,3) = z(i+lags);
    }

    //  OLS regress x(dep) on to y(indep):
    x_coefficients = arma::inv(arma::trans(xMatrix) * (xMatrix)) * arma::trans(xMatrix) * (xDiff);
    x_errors = xDiff - (xMatrix * x_coefficients);

    //  OLS regress y(dep) on to x(indep):
    y_coefficients = arma::inv(arma::trans(xMatrix) * (xMatrix)) * arma::trans(xMatrix) * (yDiff);
    y_errors = yDiff - (xMatrix * y_coefficients);

}

double Cointegration::PositionHedge(arma::vec& y, arma::vec& x)
{
    /* "Pairs Trading", Vidyamurthy, recommends:
     * use VWAP to determine beta.
     * Use the pairs combo with the higher beta coefficient to
     * assure the independent var will have the lower volatility */

    double beta;
    int rows = y.n_rows;
    arma::vec yreturns(y.n_rows-1);
    arma::vec xreturns(y.n_rows-1);

    for(int i = 0; i < (rows - 1); i++){
        yreturns(i) = y(i) - y(i + 1);
        xreturns(i) = x(i) - x(i + 1);
    }

    beta = arma::as_scalar(arma::cov(yreturns, xreturns) / arma::var(xreturns));

    return beta;

}
