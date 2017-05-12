TEMPLATE = app
QT += widgets
LIBS += -lOpenCL
TARGET = pathdrawing
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp \
           pathwidget.cpp
HEADERS += pathwidget.h
RESOURCES += pathdrawing.qrc

LIBS += -L../../../lib -L../../../bin
include(../../../src/opencl/opencl_dep.pri)
