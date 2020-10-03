
QT += core xml network testlib
CONFIG += console

TEMPLATE = app
DEFINES += "AZE_VERSION_STRING=\"\\\"0.0.0.alpha.1\\\"\""

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../AzeLib/sources

DEPENDPATH += $$PWD/../qt-plus
DEPENDPATH += $$PWD/../AzeLib

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

# Sources
include(Aze.pri)

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../AzeLib/bin -laze-dbg
} else {
    LIBS += -L$$OUT_PWD/../AzeLib/bin -laze
}

# Deployment
# In order to activate deployment, add "deploy=1" to qmake arguments
QT_BASE_PATH = $$getQtPath()
QT_BIN_PATH = $$getQtBinPath()
QT_LIB_PATH = $$getQtLibPath()

!isEmpty(deploy) {
    message("Deployment files will be copied after linkage, from $${QT_BASE_PATH}.")

    QMAKE_POST_LINK += $$copyFilesWithPathToDir($$QT_LIB_PATH, $$QT_LIB_NAMES, $$DESTDIR)
    QMAKE_POST_LINK += $$copyFilesWithPathToDir($$QT_BASE_PATH, $$QT_PLUGIN_NAMES, $$DESTDIR)
    QMAKE_POST_LINK += $$copyFilesToDir($$PWD, $$DEPLOY_NAMES, $$DESTDIR)
    QMAKE_POST_LINK += $$copyFilesToDir($$OUT_PWD, $$LIB_COPY_NAMES, $$DESTDIR)
    # QMAKE_POST_LINK += $$copyDirsToDir($$QT_BASE_PATH, $$QT_QML_NAMES, $$DESTDIR)

    unix {
        QMAKE_POST_LINK += $$makeExecutable($$EXEC_NAMES, $$DESTDIR)
    }
}
