HEADERS += \
    MainWidget.h \
    Settings.h \
    config.h \
    MainWindow.h \
    MessageBox.h

SOURCES += \
    MainWidget.cpp \
    main.cpp \
    Settings.cpp \
    MainWindow.cpp \
    MessageBox.cpp

CONFIG += C++11
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = MyQtApp
TEMPLATE = app
QT += serialport
RESOURCES += \
    qss.qrc

FORMS += \
    mainwindow.ui

RC_ICONS = serial.ico

CONFIG(debug,debug|release){

}else{
    DEFINES += QT_NO_DEBUG_OUTPUT
}
