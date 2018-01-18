######################################################################
# Automatically generated by qmake (3.0) ?? 10? 10 11:24:52 2017
######################################################################

QT += core gui serialport widgets network

target.path = /home/HAPH/haph
INSTALLS += target

contains(DEFINES,ARM){
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
           helper/Serial/deviceconfiguration.h \
           helper/Serial/deviceoperator.h \
           helper/Serial/deviceregistermap.h \
           helper/Serial/modbuscrc.h \
           helper/SwitchButton/switchbutton.h \
           setting/tableWidget/collectionsetting.h \
           setting/tableWidget/systemsetting.h \
           setting/tableWidget/zigbeesetting.h \
    helper/helper.h \
    helper/JsonConfig/dasConfig.h \
    numpad.h \
    helper/Mqtt/mqttoperator.h \
    helper/watchdog/watchdog.h \
    helper/Gpio/GPIOSet.h \
    helper/Data/dataoperator.h \
    shownum.h \
    helper/AES/aes.h \
    setting/tableWidget/print.h \
    helper/Printer/printer.h
SOURCES += main.cpp \
           mainwindow.cpp \
           timewidget.cpp \
           setting/DMSNavigation.cpp \
           setting/settingdialog.cpp \
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
           helper/Serial/deviceoperator.cpp \
           helper/Serial/modbuscrc.cpp \
           helper/SwitchButton/switchbutton.cpp \
           setting/tableWidget/collectionsetting.cpp \
           setting/tableWidget/systemsetting.cpp \
           setting/tableWidget/zigbeesetting.cpp \
    helper/helper.cpp \
    helper/JsonConfig/dasConfig.cpp \
    numpad.cpp \
    helper/Mqtt/mqttoperator.cpp \
    helper/watchdog/watchdog.cpp \
    helper/Gpio/GPIOSet.cpp \
    helper/Data/dataoperator.cpp \
    shownum.cpp \
    helper/AES/aes.cpp \
    setting/tableWidget/print.cpp \
    helper/Printer/printer.cpp
RESOURCES += pic.qrc

FORMS += \
    numpad.ui \
    setting/tableWidget/zigbeesetting.ui \
    shownum.ui \
    setting/tableWidget/print.ui
