
QT += core xml testlib network
CONFIG += console

TEMPLATE = app
DEFINES += "AZE_VERSION_STRING=\"\\\"0.0.0.alpha.1\\\"\""

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../AzeLib/sources

DEPENDPATH += $$PWD/../qt-plus
DEPENDPATH += $$PWD/../AzeLib

# Sources
include(Aze.pri)

# Functions
include($$PWD/../qt-plus/functions.pri)

# Directories
DESTDIR = $$OUT_PWD/bin

# Target binary name
CONFIG(debug, debug|release) {
    TARGET = aze-dbg
} else {
    TARGET = aze
}

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../AzeLib/bin -laze-dbg
} else {
    LIBS += -L$$OUT_PWD/../AzeLib/bin -laze
}

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}
