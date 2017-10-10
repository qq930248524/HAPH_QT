######################################################################
# Automatically generated by qmake (3.0) ?? 10? 10 11:24:52 2017
######################################################################

QT += core gui serialport widgets network

contains(QT_ARCH, armv7) {
     LIBS += -L/opt/tslib/lib/ -lts
}


TEMPLATE = app
TARGET = HAPH
INCLUDEPATH += .

# Input
HEADERS += mainwindow.h \
           timewidget.h \
           setting/DMSNavigation.h \
           setting/settingdialog.h \
           setting/settingwidget.h \
           helper/Mqtt/qmqtt.h \
           helper/Mqtt/qmqtt_client.h \
           helper/Mqtt/qmqtt_client_p.h \
           helper/Mqtt/qmqtt_frame.h \
           helper/Mqtt/qmqtt_global.h \
           helper/Mqtt/qmqtt_message.h \
           helper/Mqtt/qmqtt_message_p.h \
           helper/Mqtt/qmqtt_network_p.h \
           helper/Mqtt/qmqtt_networkinterface.h \
           helper/Mqtt/qmqtt_routedmessage.h \
           helper/Mqtt/qmqtt_router.h \
           helper/Mqtt/qmqtt_routesubscription.h \
           helper/Mqtt/qmqtt_socket_p.h \
           helper/Mqtt/qmqtt_socketinterface.h \
           helper/Mqtt/qmqtt_ssl_socket_p.h \
           helper/Mqtt/qmqtt_timer_p.h \
           helper/Mqtt/qmqtt_timerinterface.h \
           helper/QGauge/qgauge.h \
           helper/QMeter/qmeter.h \
           helper/Serial/deviceconfiguration.h \
           helper/Serial/deviceoperator.h \
           helper/Serial/deviceregistermap.h \
           helper/Serial/modbuscrc.h \
           helper/SwitchButton/switchbutton.h \
           setting/tableWidget/collectionsetting.h \
           setting/tableWidget/systemsetting.h \
           setting/tableWidget/zigbeesetting.h
SOURCES += main.cpp \
           mainwindow.cpp \
           timewidget.cpp \
           setting/DMSNavigation.cpp \
           setting/settingdialog.cpp \
           setting/settingwidget.cpp \
           helper/Mqtt/qmqtt_client.cpp \
           helper/Mqtt/qmqtt_client_p.cpp \
           helper/Mqtt/qmqtt_frame.cpp \
           helper/Mqtt/qmqtt_message.cpp \
           helper/Mqtt/qmqtt_network.cpp \
           helper/Mqtt/qmqtt_router.cpp \
           helper/Mqtt/qmqtt_routesubscription.cpp \
           helper/Mqtt/qmqtt_socket.cpp \
           helper/Mqtt/qmqtt_ssl_socket.cpp \
           helper/Mqtt/qmqtt_timer.cpp \
           helper/QGauge/qgauge.cpp \
           helper/QGauge/qgaugedrawfunctions.cpp \
           helper/QMeter/qmeter.cpp \
           helper/QMeter/qmeterdrawfunctions.cpp \
           helper/Serial/deviceoperator.cpp \
           helper/Serial/modbuscrc.cpp \
           helper/SwitchButton/switchbutton.cpp \
           setting/tableWidget/collectionsetting.cpp \
           setting/tableWidget/systemsetting.cpp \
           setting/tableWidget/zigbeesetting.cpp
RESOURCES += pic.qrc
