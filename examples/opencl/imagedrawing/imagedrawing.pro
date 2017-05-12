TEMPLATE = app
TARGET = imagedrawing
QT += widgets
LIBS += -lOpenCL
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp \
           imagewidget.cpp
HEADERS += imagewidget.h
RESOURCES += imagedrawing.qrc

LIBS += -L../../../lib -L../../../bin
include(../../../src/opencl/opencl_dep.pri)
