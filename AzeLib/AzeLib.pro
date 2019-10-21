
QT += core xml

TEMPLATE = lib
DEFINES += "AZELIB_VERSION_STRING=\"\\\"0.0.0.alpha.1\\\"\""
DEFINES += AZE_LIBRARY
DEFINES += QTPLUS_LIBRARY
DEFINES += CXMLNODE_USE_LIST

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../dtl/dtl

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
