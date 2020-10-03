
# Sources

HEADERS += \
    sources/server/CAzeServer.h \
    sources/server/CAzeServerArguments.h \
    sources/server/CAzeServerProcessor.h \
    sources/client/CAzeClient.h \
    sources/client/CAzeClientArguments.h \
    sources/common/CConstants.h \
    sources/tests/CTestAze.h \

SOURCES += \
    sources/server/CAzeServer.cpp \
    sources/server/CAzeServerArguments.cpp \
    sources/server/CAzeServerProcessor.cpp \
    sources/client/CAzeClient.cpp \
    sources/client/CAzeClient_Local.cpp \
    sources/client/CAzeClient_Remote.cpp \
    sources/client/CAzeClientArguments.cpp \
    sources/common/CConstants.cpp \
    sources/tests/CTestAze.cpp \
    sources/main.cpp \

# Deployment

unix {
    QT_LIB_NAMES = \
        libicudata.so.56 \
        libicui18n.so.56 \
        libicuuc.so.56 \
        libQt5Core.so.5 \
        libQt5Gui.so.5 \
        libQt5Network.so.5 \
        libQt5XcbQpa.so.5 \
        libQt5Xml.so.5

    QT_PLUGIN_NAMES = \
        plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so \
        plugins/platforms/libqxcb.so \
        plugins/xcbglintegrations/libqxcb-egl-integration.so \
        plugins/xcbglintegrations/libqxcb-glx-integration.so

    DEPLOY_NAMES = \
        deploy/linux/Aze.sh \
        deploy/linux/qt.conf

    LIB_COPY_NAMES =

    EXEC_NAMES = \
        CuteGit.sh
} else {
    CONFIG(debug, debug|release) {
        QT_LIB_NAMES = \
            libgcc_s_seh-1.dll \
            libwinpthread-1.dll \
            # libstdc++-6.dll \
            Qt5Cored.dll \
            Qt5Guid.dll \
            Qt5Networkd.dll \
            Qt5Testd.dll \
            Qt5Xmld.dll
    } else {
        QT_LIB_NAMES = \
            libgcc_s_seh-1.dll \
            libwinpthread-1.dll \
            # libstdc++-6.dll \
            Qt5Core.dll \
            Qt5Gui.dll \
            Qt5Network.dll \
            Qt5Test.dll \
            Qt5Xml.dll
    }

    QT_PLUGIN_NAMES = \
        plugins/platforms/qwindows.dll

    DEPLOY_NAMES = \
        deploy/windows/qt.conf

    LIB_COPY_NAMES = \
        ../AzeLib/bin/$$TARGET.dll

    EXEC_NAMES =
}

QT_QML_NAMES =
