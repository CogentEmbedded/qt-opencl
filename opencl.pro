TEMPLATE = subdirs
SUBDIRS = src examples demos util tests

load(qt_parts)

CONFIG += ordered

include(doc/doc.pri)
