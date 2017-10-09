#-------------------------------------------------
#
# Project created by QtCreator 2017-09-10T17:47:22
#
#-------------------------------------------------

QT       += core gui serialport widgets network


contains(QT_ARCH, armv7) {
     LIBS += -L/opt/tslib/lib/ -lts
}

TARGET = HAHP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
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
    helper/SwitchButton/switchbutton.cpp \
    helper/mqtt/qmqtt_client_p.cpp \
    helper/mqtt/qmqtt_client.cpp \
    helper/mqtt/qmqtt_frame.cpp \
    helper/mqtt/qmqtt_message.cpp \
    helper/mqtt/qmqtt_network.cpp \
    helper/mqtt/qmqtt_router.cpp \
    helper/mqtt/qmqtt_routesubscription.cpp \
    helper/mqtt/qmqtt_socket.cpp \
    helper/mqtt/qmqtt_ssl_socket.cpp \
    helper/mqtt/qmqtt_timer.cpp

HEADERS  += mainwindow.h \
    passwidget.h \
    timewidget.h \
    DMSNavigation.h \
    setting/DMSNavigation.h \
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
    helper/SwitchButton/switchbutton.h \
    helper/mqtt/qmqtt_client_p.h \
    helper/mqtt/qmqtt_client.h \
    helper/mqtt/qmqtt_frame.h \
    helper/mqtt/qmqtt_global.h \
    helper/mqtt/qmqtt_message_p.h \
    helper/mqtt/qmqtt_message.h \
    helper/mqtt/qmqtt_network_p.h \
    helper/mqtt/qmqtt_networkinterface.h \
    helper/mqtt/qmqtt_routedmessage.h \
    helper/mqtt/qmqtt_router.h \
    helper/mqtt/qmqtt_routesubscription.h \
    helper/mqtt/qmqtt_socket_p.h \
    helper/mqtt/qmqtt_socketinterface.h \
    helper/mqtt/qmqtt_ssl_socket_p.h \
    helper/mqtt/qmqtt_timer_p.h \
    helper/mqtt/qmqtt_timerinterface.h \
    helper/mqtt/qmqtt.h

RESOURCES += \
    pic.qrc

FORMS +=



INCLUDEPATH += $$PWD/../../../usr/local/tslib/include
DEPENDPATH += $$PWD/../../../usr/local/tslib/include

SUBDIRS += \
    helper/mqtt/mqtt.pro

DISTFILES += \
    helper/mqtt/mqtt.pri \
    helper/mqtt/mqtt.qbs \
    helper/mqtt/mqttModule.qbs \
    helper/mqtt/mqtt.pro.user
