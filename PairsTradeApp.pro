#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T10:25:56
# Rodney Purkey
#
#-------------------------------------------------

QT       += core gui sql network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PairsTradeApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cointegration.cpp \
    dbconnect.cpp \
    mastersymbollist.cpp \
    getprices.cpp \
    datesetter.cpp \
    datatablemodel.cpp \
    statistics.cpp


HEADERS  += mainwindow.h \
    cointegration.h \
    dbconnect.h \
    mastersymbollist.h \
    getprices.h \
    datesetter.h \
    datatablemodel.h \
    statistics.h

FORMS    += mainwindow.ui \
    mastersymbollist.ui \
    datesetter.ui

QMAKE_LIBS += -L/usr/lib/ -larmadillo -lopenblas #-llapack -lblas

QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS_CXX11 += -std=c++11

#-------------------------------------------------------------
#As Armadillo is a template library, we strongly recommended to have optimisation enabled when
#compiling programs (eg. when compiling with GCC, use the -O1 or -O2 options).
QMAKE_CXXFLAGS_RELEASE *= -O2
#QMAKE_CXXFLAGS_DEBUG *= -O2
#--------------------------------------------------------------

#Armadillo depends on lapack and openblas libs
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/openblas-base/release/ -lopenblas
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/openblas-base/debug/ -lopenblas
#else:unix: LIBS += -L$$PWD/../../../../../usr/lib/openblas-base/ -lopenblas
INCLUDEPATH += $$PWD/../../../../../usr/lib
DEPENDPATH += $$PWD/../../../../../usr/lib

#libcurl (cURL) is a client-side URL transfer library. libcurl is portable and bindings are
#available in C/C++. libcurl (not cURL) supports simultaneous downloads of multiple URLs.
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/release/ -lcurl
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/debug/ -lcurl
#else:unix: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lcurl
#INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
#DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu

#curlpp is C++ wrapper to libcurl.
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lcurlpp
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lcurlpp
#else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lcurlpp

INCLUDEPATH += $$PWD/../../../../../usr/local/lib
DEPENDPATH += $$PWD/../../../../../usr/local/lib
