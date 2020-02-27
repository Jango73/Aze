
// Qt
#include <QDebug>
#include <QDir>
#include <QDirIterator>

// qt-plus
#include <CXMLNode.h>
#include <File/CFileUtilities.h>

// Aze
#include "CUtils.h"

// Application
#include "CAzeClient.h"

/*!
    \class CAzeClient
    \inmodule Aze
    \section1 General
*/

// Server IP
#define SERVER_IP "127.0.0.1"

// Port number
#define PORT_NUMBER "8080"

//-------------------------------------------------------------------------------------------------

CAzeClient::CAzeClient(int argc, char *argv[], QTextStream* pOutStream)
    : QCoreApplication(argc, argv)
    , m_tArguments(*this)
    , m_pOutStream(pOutStream)
    , m_pRepository(new Aze::CRepository(QDir::currentPath(), this))
    , m_pNetworkAccessManager(new QNetworkAccessManager(this))
    , m_bOutputStreamIsMine(false)
    , m_bNetworkAccessFinished(false)
{
    if (m_pOutStream == nullptr)
    {
        m_bOutputStreamIsMine = true;
        m_pOutStream = new QTextStream(stdout);
    }

    // Setup network request
    QString sUrl = QString("http://%1:%2").arg(SERVER_IP).arg(PORT_NUMBER);
    m_tNetworkRequest.setUrl(QUrl(sUrl));
    m_tNetworkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Listen to network manager responses
    connect(m_pNetworkAccessManager, &QNetworkAccessManager::finished, this, &CAzeClient::onNetworkAccessFinished);
}

//-------------------------------------------------------------------------------------------------

CAzeClient::~CAzeClient()
{
    if (m_bOutputStreamIsMine)
        delete m_pOutStream;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::run()
{
    switch (m_tArguments.m_eCommand)
    {
    case CConstants::eCommandNone: break;

    // Local commands
    case CConstants::eCommandCleanUp:        return cleanUp();
    case CConstants::eCommandCommit:         return commit();
    case CConstants::eCommandCreateBranch:   return createBranch();
    case CConstants::eCommandDiff:           return diff();
    case CConstants::eCommandDump:           return dump();
    case CConstants::eCommandInitRepository: return init();
    case CConstants::eCommandLog:            return log();
    case CConstants::eCommandMerge:          return merge();
    case CConstants::eCommandMove:           return move();
    case CConstants::eCommandPopStash:       return popStash();
    case CConstants::eCommandRemove:         return remove();
    case CConstants::eCommandRevert:         return revert();
    case CConstants::eCommandSaveStash:      return saveStash();
    case CConstants::eCommandShowStatus:     return status();
    case CConstants::eCommandStage:          return stage();
    case CConstants::eCommandSwitchToBranch: return switchToBranch();
    case CConstants::eCommandUnstage:        return unstage();

    // Remote commands
    case CConstants::eCommandPush:           return push();
    }

    OUT_ERROR("Unknown command...");

    return 0;
}

//-------------------------------------------------------------------------------------------------

bool CAzeClient::isASainRepository()
{
    if (not m_pRepository->ok())
    {
        OUT_ERROR(Aze::CStrings::s_sTextNotARepository);

        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

void CAzeClient::processWildCards()
{
    m_tArguments.m_lFilesAndIds = CFileUtilities::getInstance()->concernedFiles(
                m_pRepository->database()->startPath(),
                m_tArguments.m_lFilesAndIds
                );
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::help()
{
    (*m_pOutStream) << CConstants::s_sTextCommands << ":\n\n";

    for (QString sKey : CConstants::s_mHelp.keys())
    {
        (*m_pOutStream) << QString(" %1  %2\n").arg(sKey, -15).arg(CConstants::s_mHelp[sKey]);
    }

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------
