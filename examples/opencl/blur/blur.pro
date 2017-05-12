TEMPLATE = app
TARGET = blur
QT += widgets
LIBS += -lOpenCL
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp \
           blurwidget.cpp
HEADERS += blurwidget.h \
           blurtable.h
RESOURCES += blur.qrc

LIBS += -L../../../lib -L../../../bin
include(../../../src/opencl/opencl_dep.pri)
