
// Qt
#include <QDebug>

// Application
#include "CAzeClientArguments.h"

/*!
    \class CAzeClientArguments
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

CAzeClientArguments::CAzeClientArguments(QCoreApplication& app)
    : QObject(nullptr)
    , m_oAll(QStringList() << CConstants::s_sSwitchAll,
             QCoreApplication::translate(CConstants::s_sContextMain, "Include all files."))
    , m_oLoose(QStringList() << CConstants::s_sSwitchLoose,
               QCoreApplication::translate(CConstants::s_sContextMain, "Include loose files."))
    , m_oClean(QStringList() << CConstants::s_sSwitchClean,
               QCoreApplication::translate(CConstants::s_sContextMain, "Include clean files."))
    , m_oModified(QStringList() << CConstants::s_sSwitchModified,
                  QCoreApplication::translate(CConstants::s_sContextMain, "Include modified files."))
    , m_oAdded(QStringList() << CConstants::s_sSwitchAdded,
               QCoreApplication::translate(CConstants::s_sContextMain, "Include added files."))
    , m_oDeleted(QStringList() << CConstants::s_sSwitchDeleted,
                 QCoreApplication::translate(CConstants::s_sContextMain, "Include deleted files."))
    , m_oMissing(QStringList() << CConstants::s_sSwitchMissing,
                 QCoreApplication::translate(CConstants::s_sContextMain, "Include missing files."))
    , m_oIgnored(QStringList() << CConstants::s_sSwitchIgnored,
                 QCoreApplication::translate(CConstants::s_sContextMain, "Include ignored files."))
    , m_oAuthor(QStringList() << "a" << CConstants::s_sSwitchAuthor,
                QCoreApplication::translate(CConstants::s_sContextMain, "Set commit author to <author>."),
                QCoreApplication::translate(CConstants::s_sContextMain, CConstants::s_sSwitchAuthor))
    , m_oMessage(QStringList() << "m" << CConstants::s_sSwitchMessage,
                 QCoreApplication::translate(CConstants::s_sContextMain, "Set commit message to <message>."),
                 QCoreApplication::translate(CConstants::s_sContextMain, CConstants::s_sSwitchMessage))
    , m_oStart(QStringList() << "s" << CConstants::s_sSwitchStart,
               QCoreApplication::translate(CConstants::s_sContextMain, "Begin listing after <start> items."),
               QCoreApplication::translate(CConstants::s_sContextMain, CConstants::s_sSwitchStart))
    , m_oCount(QStringList() << "c" << CConstants::s_sSwitchCount,
               QCoreApplication::translate(CConstants::s_sContextMain, "List <count> items."),
               QCoreApplication::translate(CConstants::s_sContextMain, CConstants::s_sSwitchCount))
    , m_oAllowFileDelete(QStringList() << "d" << CConstants::s_sSwitchAllowFileDelete,
                         QCoreApplication::translate(CConstants::s_sContextMain, "Allow Aze to delete files."))
    , m_oGraph(QStringList() << "g" << CConstants::s_sSwitchGraph,
               QCoreApplication::translate(CConstants::s_sContextMain, "Show log as a graph."))
    , m_eCommand(CConstants::eCommandNone)
{
    CConstants::initCommandMap();

    // Application name and version
    QCoreApplication::setApplicationName("Aze");
    QCoreApplication::setApplicationVersion(AZE_VERSION_STRING);

    // Set option array
    m_tParser.addOptions({
                             m_oAll,
                             m_oLoose,
                             m_oClean,
                             m_oModified,
                             m_oAdded,
                             m_oDeleted,
                             m_oMissing,
                             m_oIgnored,
                             m_oAuthor,
                             m_oMessage,
                             m_oStart,
                             m_oCount,
                             m_oAllowFileDelete,
                             m_oGraph
                         });

    // Finalize option array
    m_tParser.addHelpOption();
    m_tParser.addVersionOption();
    m_tParser.process(app);

    // Get files and Ids
    m_lFilesAndIds = m_tParser.positionalArguments();

    // Get command
    if (m_lFilesAndIds.count() > 0)
    {
        QString sCommand = m_lFilesAndIds.takeFirst();
        m_eCommand = CConstants::s_mCommands[sCommand];
    }
}

//-------------------------------------------------------------------------------------------------
