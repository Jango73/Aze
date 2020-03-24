
// Qt
#include <QDebug>

// qt-plus
#include "Web/CHTTPServer.h"

// Application
#include "CAzeServerArguments.h"

/*!
    \class CAzeServerArguments
    \inmodule Aze
    \section1 General
*/

#define TRT(a) QCoreApplication::translate(CConstants::s_sContextMain, a)

//-------------------------------------------------------------------------------------------------

CAzeServerArguments::CAzeServerArguments(QCoreApplication& app)
    : QObject(nullptr)
    , m_oPort(QStringList() << CConstants::s_sSwitchPort << "p", TRT("Listening port."), TRT(CConstants::s_sSwitchPort), QString::number(DEFAULT_HTTP_PORT))
{
    CConstants::initCommandMap();

    // Application name and version
    QCoreApplication::setApplicationName("Aze");
    QCoreApplication::setApplicationVersion(AZE_VERSION_STRING);

    // Set option array
    m_tParser.addOptions({ m_oPort });

    // Finalize option array
    m_tParser.addHelpOption();
    m_tParser.addVersionOption();
    m_tParser.process(app);
}

//-------------------------------------------------------------------------------------------------
