
// Qt
#include <QDebug>

// Application
#include "CAzeServerArguments.h"

/*!
    \class CAzeServerArguments
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

CAzeServerArguments::CAzeServerArguments(QCoreApplication& app)
    : QObject(nullptr)
    , m_oPort(QStringList() << CConstants::s_sSwitchPort,
             QCoreApplication::translate(CConstants::s_sContextMain, "Listening port."))
{
    CConstants::initCommandMap();

    // Application name and version
    QCoreApplication::setApplicationName("Aze");
    QCoreApplication::setApplicationVersion(AZE_VERSION_STRING);

    // Set option array
    m_tParser.addOptions({
                             m_oPort
                         });

    // Finalize option array
    m_tParser.addHelpOption();
    m_tParser.addVersionOption();
    m_tParser.process(app);
}

//-------------------------------------------------------------------------------------------------
