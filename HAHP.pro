#-------------------------------------------------
#
# Project created by QtCreator 2017-09-10T17:47:22
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HAHP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    passwidget.cpp \
    timewidget.cpp \
    setting/DMSNavigation.cpp \
    setting/settingdialog.cpp \
    setting/tableWidget/systemsetting.cpp \
    setting/tableWidget/collectionsetting.cpp \
    setting/tableWidget/zigbeesetting.cpp \
    helper/deviceoperator.cpp \
    helper/modbuscrc.cpp \
    helper/QGauge/qgauge.cpp \
    helper/QGauge/qgaugedrawfunctions.cpp \
    helper/QMeter/qmeter.cpp \
    helper/QMeter/qmeterdrawfunctions.cpp \
    helper/SwitchButton/switchbutton.cpp

HEADERS  += mainwindow.h \
    passwidget.h \
    timewidget.h \
    DMSNavigation.h \
    setting/DMSNavigation.h \
    settingwidget.h \
    setting/settingdialog.h \
    setting/tableWidget/systemsetting.h \
    setting/tableWidget/collectionsetting.h \
    setting/tableWidget/zigbeesetting.h \
    setting/tableWidget/deviceoperator.h \
    helper/deviceoperator.h \
    helper/modbuscrc.h \
    helper/deviceregistermap.h \
    helper/deviceconfiguration.h \
    helper/QGauge/qgauge.h \
    helper/QMeter/qmeter.h \
    helper/SwitchButton/switchbutton.h

RESOURCES += \
    pic.qrc \
    passwidget.qrc

FORMS +=

#unix:!macx: LIBS += -L$$PWD/../../../usr/local/tslib/lib/ -lts

INCLUDEPATH += $$PWD/../../../usr/local/tslib/include
DEPENDPATH += $$PWD/../../../usr/local/tslib/include
