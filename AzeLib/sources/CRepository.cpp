
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRepository.h"
#include "CUtils.h"

#include "commands/CCommitCommand.h"
#include "commands/CDiffCommand.h"
#include "commands/CLogCommand.h"
#include "commands/CMergeCommand.h"
#include "commands/CPopStashCommand.h"
#include "commands/CRemoveCommand.h"
#include "commands/CRevertCommand.h"
#include "commands/CSaveStashCommand.h"
#include "commands/CStageCommand.h"
#include "commands/CStatusCommand.h"
#include "commands/CSwitchToBranchCommand.h"
#include "commands/CUnstageCommand.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRepository::CRepository(const QString& sRootPath, QObject* parent)
    : CObject(parent)
    , m_bOk(false)
    , m_pDatabase(new CDatabase(sRootPath, this))
    , m_pRemoteHostInfo(new CRemoteHostInfo(this))
    , m_pCommitFunctions(new CCommitFunctions(m_pDatabase, this))
    , m_eStatus(CEnums::eUnknown)
    , m_pCurrentBranch(nullptr)
    , m_pStagingCommit(nullptr)
    , m_pRootCommit(nullptr)
    , m_pTipCommit(nullptr)
{
    // Get the path of the staging file
    m_sStagingCommitFileName = m_pDatabase->composeCommitFileName(CStrings::s_sStagingCommitFileName);

    // Check if repository is ok
    QDir dataPath(m_pDatabase->dataPath());
    if (dataPath.exists())
        setOk(true);

    if (m_bOk)
    {
        readGeneralInformation();
        readCurrentBranch();
        readTipCommit();
    }
}

//-------------------------------------------------------------------------------------------------

CRepository::~CRepository()
{
}

//-------------------------------------------------------------------------------------------------

bool CRepository::init()
{
    if (m_pDatabase->init())
    {
        createBranch(CStrings::s_sDefaultBranchName);

        QString sBranchFileName = m_pDatabase->composeBranchFileName(CStrings::s_sDefaultBranchName);
        setCurrentBranch(CBranch::fromNode(CXMLNode::load(sBranchFileName), this));
        setCurrentBranchName(CStrings::s_sDefaultBranchName);

        writeGeneralInformation();
        writeCurrentBranch();

        setOk(true);

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::createBranch(const QString& sName)
{
    QString sBranchFileName = m_pDatabase->composeBranchFileName(sName);

    if (QFile(sBranchFileName).exists())
    {
        OUT_INFO(CStrings::s_sTextBranchAlreadyExists);
    }
    else
    {
        CBranch* pNewBranch = new CBranch(this);

        if (not IS_NULL(m_pCurrentBranch))
        {
            pNewBranch->setRootCommitId(m_pCurrentBranch->tipCommitId());
            pNewBranch->setTipCommitId(m_pCurrentBranch->tipCommitId());
        }

        pNewBranch->toNode().save(sBranchFileName);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::switchToBranch(const QString& sName, bool bAllowFileDelete)
{
    return CSwitchToBranchCommand(this, sName, bAllowFileDelete).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::stage(const QStringList& lFileNames)
{
    return CStageCommand(this, lFileNames).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::unstage(const QStringList& lFileNames)
{
    return CUnstageCommand(this, lFileNames).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::remove(const QStringList& lFileNames)
{
    return CRemoveCommand(this, lFileNames).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::commit(const QString& sAuthor, const QString& sMessage)
{
    return CCommitCommand(this, sAuthor, sMessage).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::revert(const QStringList& lFileNames, bool bAllowFileDelete)
{
    return CRevertCommand(this, lFileNames, bAllowFileDelete).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::revert(CCommit* pWorkingDirectory, bool bAllowFileDelete)
{
    return CRevertCommand(this, pWorkingDirectory, bAllowFileDelete).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::revert(bool bAllowFileDelete)
{
    CCommit* pWorkingDirectory = m_pCommitFunctions->directoryAsCommit();
    return CRevertCommand(this, pWorkingDirectory, bAllowFileDelete).execute();
}

//-------------------------------------------------------------------------------------------------

QString CRepository::log(const QStringList& lFileNames, bool bGraph, int iStart, int iCount)
{
    QString sReturnValue;

    if (CLogCommand(this, lFileNames, &sReturnValue, bGraph, iStart, iCount).execute())
    {
        return sReturnValue;
    }

    return "";
}

//-------------------------------------------------------------------------------------------------

QString CRepository::diff(const QString& sFirst, const QString& sSecond)
{
    QString sReturnValue;

    if (CDiffCommand(this, sFirst, sSecond, &sReturnValue).execute())
    {
        return sReturnValue;
    }

    return "";
}

//-------------------------------------------------------------------------------------------------

bool CRepository::merge(const QString& sName)
{
    return CMergeCommand(this, sName).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::saveStash(const QString& sMessage)
{
    return CSaveStashCommand(this, sMessage).execute();
}

//-------------------------------------------------------------------------------------------------

bool CRepository::popStash(const QString& sId)
{
    return CPopStashCommand(this, sId).execute();
}

//-------------------------------------------------------------------------------------------------

QList<CFile> CRepository::fileStatus(const QStringList& lFileNames)
{
    m_eStatus = checkStatus(lFileNames);
    return m_lFiles;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readGeneralInformation()
{
    CXMLNode xInfo = CXMLNode::load(m_pDatabase->generalInformationFileName());

    // Read remote host information
    CXMLNode xRemoteHost = xInfo.getNodeByTagName(CStrings::s_sParamRemoteHost);
    m_pRemoteHostInfo->setName(xRemoteHost.attributes()[CStrings::s_sParamName]);

    // Read branches
    CXMLNode xBranches = xInfo.getNodeByTagName(CStrings::s_sParamBranches);
    setCurrentBranchName(xBranches.attributes()[CStrings::s_sParamCurrent]);

    // Read stash list
    CXMLNode xStashListNode = xInfo.getNodeByTagName(CStrings::s_sParamStashList);
    CXMLNodeList xStashList = xStashListNode.getNodesByTagName(CStrings::s_sParamStash);

    for (CXMLNode& xStash : xStashList)
    {
        m_lStashList << xStash.attributes()[CStrings::s_sParamId];
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readCurrentBranch()
{
    QString sFileName = m_pDatabase->composeBranchFileName(m_sCurrentBranchName);
    setCurrentBranch(CBranch::fromNode(CXMLNode::load(sFileName), this));

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readStage()
{
    if (IS_NULL(m_pStagingCommit))
    {
        setStagingCommit(CCommit::fromFile(m_sStagingCommitFileName, this, ""));
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readRootCommit()
{
    if (not m_pCurrentBranch->rootCommitId().isEmpty())
    {
        delete m_pRootCommit;
        setRootCommit(m_pDatabase->getCommit(m_pCurrentBranch->rootCommitId(), this));
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readTipCommit()
{
    if (not m_pCurrentBranch->tipCommitId().isEmpty())
    {
        delete m_pTipCommit;
        setTipCommit(m_pDatabase->getCommit(m_pCurrentBranch->tipCommitId(), this));
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::writeGeneralInformation()
{
    CXMLNode xInfo(CStrings::s_sParamInfo);

    // Write remote host information
    CXMLNode xRemoteHost(CStrings::s_sParamRemoteHost);
    xRemoteHost.attributes()[CStrings::s_sParamName] = m_pRemoteHostInfo->name();
	xInfo << xRemoteHost;

    // Write branches
    CXMLNode xBranches(CStrings::s_sParamBranches);
    xBranches.attributes()[CStrings::s_sParamCurrent] = m_sCurrentBranchName;
    xInfo << xBranches;

    // Write stash list
    CXMLNode xStashListNode(CStrings::s_sParamStashList);
    for (const QString& sId : m_lStashList)
    {
        CXMLNode xStash(CStrings::s_sParamStash);
        xStash.attributes()[CStrings::s_sParamId] = sId;
        xStashListNode << xStash;
    }

    xInfo << xStashListNode;

    xInfo.save(m_pDatabase->generalInformationFileName());

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::writeCurrentBranch()
{
    if (m_pCurrentBranch != nullptr)
        m_pCurrentBranch->toNode().save(m_pDatabase->composeBranchFileName(m_sCurrentBranchName));

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::writeStage()
{
    if (not IS_NULL(m_pStagingCommit))
    {
        m_pStagingCommit->toFile(m_sStagingCommitFileName);
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::clearStage()
{
    if (not IS_NULL(m_pStagingCommit))
    {
        delete m_pStagingCommit;
        m_pStagingCommit = new CCommit(this);

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

CEnums::EFileStatus CRepository::checkStatus(const QStringList& lFileNames)
{
    if (m_eStatus == CEnums::eUnknown)
    {
        CStatusCommand(this, lFileNames, &m_lFiles, &m_eStatus).execute();
    }

    return m_eStatus;
}

//-------------------------------------------------------------------------------------------------

QStringList CRepository::getLooseFiles()
{
    QStringList lReturnValue;

    // Check status of working directory
    checkStatus(QStringList());

    for (CFile& tFile : m_lFiles)
        if (tFile.status() == CEnums::eLoose)
            lReturnValue << tFile.relativeName();

    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CRepository::addStashToList(const QString& sId)
{
    if (not sId.isEmpty())
        m_lStashList << sId;
}

//-------------------------------------------------------------------------------------------------

void CRepository::removeStashFromList(const QString& sId)
{
    m_lStashList.removeAll(sId);
}

//-------------------------------------------------------------------------------------------------

void CRepository::setRemoteHostName(const QString& sName)
{
    m_pRemoteHostInfo->setName(sName);
}

//-------------------------------------------------------------------------------------------------

QString CRepository::processKeywords(const QString& sText)
{
    if (sText == CStrings::s_sParamTip && not IS_NULL(m_pCurrentBranch))
        return m_pCurrentBranch->tipCommitId();

    if (sText == CStrings::s_sParamRoot && not IS_NULL(m_pCurrentBranch))
        return m_pCurrentBranch->rootCommitId();

    return sText;
}

//-------------------------------------------------------------------------------------------------

QString CRepository::processDeltas(const QString& sText, int& iDelta)
{
    iDelta = 0;

    if (sText.contains("~"))
    {
        QStringList lValues = sText.split("~");

        if (lValues.count() > 1)
            iDelta = lValues[1].toInt();

        return lValues[0];
    }

    return sText;
}

//-------------------------------------------------------------------------------------------------

QString CRepository::diffWorkingDirectory()
{
    QStringList lLooseFiles = getLooseFiles();

    QString sDiff;
    CCommit* pFromCommit = nullptr;
    CCommit* pToCommit = nullptr;

    if (not m_pCurrentBranch->tipCommitId().isEmpty())
    {
        // Get the current branch tip commit
        pFromCommit = m_pDatabase->getCommit(m_pCurrentBranch->tipCommitId(), this);

        // Get the working directory as commit
        pToCommit = m_pCommitFunctions->directoryAsCommit(this);

        // Diff the current tip commit and the working directory
        m_pCommitFunctions->diffCommits(sDiff, pFromCommit, pToCommit, lLooseFiles);
    }

    return sDiff;
}

//-------------------------------------------------------------------------------------------------

}
