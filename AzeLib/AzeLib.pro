
QT += core xml testlib

TEMPLATE = lib
DEFINES += "AZELIB_VERSION_STRING=\"\\\"0.0.0.alpha.1\\\"\""
DEFINES += AZE_LIBRARY
DEFINES += CXMLNODE_USE_LIST

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/sources/google

# Sources
include(AzeLib.pri)

# Functions
include($$PWD/../qt-plus/functions.pri)

# Directories
DESTDIR = $$OUT_PWD/bin

# Target
CONFIG(debug, debug|release) {
    TARGET = aze-dbg
} else {
    TARGET = aze
}

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}

# Copy qt-plus to bin
copyfile = $$PWD/../qt-plus/bin/*
copydest = $$DESTDIR

QMAKE_PRE_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)
