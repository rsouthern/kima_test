QT += core network
QT -= gui

TARGET = kima_test
CONFIG += console
CONFIG -= app_bundle
OBJECTS_DIR = obj
MOC_DIR = moc

TEMPLATE = app

LIBS += -L$$(OSCLIBDIR) -losc
QMAKE_RPATHDIR = $$(OSCLIBDIR)
INCLUDEPATH += $$(OSCDIR)

SOURCES += src/main.cpp \
    src/loopback.cpp

HEADERS += \
    src/loopback.h

