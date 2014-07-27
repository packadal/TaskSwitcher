TEMPLATE = app

QT += qml quick widgets winextras gui-private

SOURCES += main.cpp \
    windowmodel.cpp \
    thumbnail.cpp \
    xboxcontrolleritem.cpp \
    desktopactions.cpp

RESOURCES += qml.qrc

CONFIG += c++11

#LIBS += "C:\Program Files (x86)\Windows Kits\8.0\Lib\win8\um\x86\dwmapi.lib"
#LIBS += C:/Windows/System32/dwmapi.dll

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    windowmodel.h \
    thumbnail.h \
    xboxcontrolleritem.h \
    desktopactions.h
