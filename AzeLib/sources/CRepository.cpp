
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRepository.h"
#include "CUtils.h"

#include "commands/CSwitchToBranchCommand.h"
#include "commands/CStageCommand.h"
#include "commands/CUnstageCommand.h"
#include "commands/CRemoveCommand.h"
#include "commands/CCommitCommand.h"
#include "commands/CRevertCommand.h"
#include "commands/CStatusCommand.h"
#include "commands/CLogCommand.h"
#include "commands/CDiffCommand.h"
#include "commands/CMergeCommand.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRepository::CRepository(const QString& sRootPath, QObject* parent)
    : CObject(parent)
    , m_bOk(false)
    , m_pDatabase(new CDatabase(sRootPath, this))
    , m_pCommitFunctions(new CCommitFunctions(m_pDatabase, this))
    , m_eStatus(CEnums::eUnknown)
    , m_pCurrentBranch(nullptr)
    , m_pStagingCommit(nullptr)
    , m_pRootCommit(nullptr)
    , m_pTipCommit(nullptr)
{
    m_sStagingCommitFileName = m_pDatabase->composeCommitFileName(CStrings::s_sStagingCommitFileName);

    // Check if repository is ok
    QDir dataPath(m_pDatabase->dataPath());
    if (dataPath.exists())
        setOk(true);

    if (m_bOk)
    {
        readGeneralInfo();
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

        writeGeneralInfo();
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

QList<QPair<QString, QString> > CRepository::splitDiff(const QString& sFullDiff)
{
    QList<QPair<QString, QString> > mReturnValue;

    QRegExp tRegExp1(QString("%1\\s+[a-zA-Z0-9\\.\\/\\+-_]+\\s+[a-zA-Z0-9\\.\\/\\+-_]+\\s*\\r*\\n")
                    .arg(CStrings::s_sDiffChunkHeader)
                    );
    QRegExp tRegExp2(QString("---\\s+([a-zA-Z0-9\\.\\/\\+-_]+)\\s*\\r*\\n"));
    QRegExp tRegExp3(QString("\\+\\+\\+\\s+([a-zA-Z0-9\\.\\/\\+-_]+)\\s*\\r*\\n"));

    // Get a list of file diffs from the full diff
    QStringList lFileDiffs = sFullDiff.split(tRegExp1, QString::SkipEmptyParts);

    // Iterate through each file diffs
    for (QString sFileDiff : lFileDiffs)
    {
        if (tRegExp2.indexIn(sFileDiff) != -1)
        {
            QString sFileName = tRegExp2.cap(1).trimmed();

            if (not sFileName.isEmpty())
            {
                // Remove things not expected by diff-match-patch
                sFileDiff.replace(tRegExp2, "");
                sFileDiff.replace(tRegExp3, "");

                mReturnValue << QPair<QString, QString>(sFileName, sFileDiff);
            }
        }
    }

    return mReturnValue;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::applyDiff(const QString& sFullDiff, bool bAddToStage)
{
    // Keep track of merged files
    QDictionary mProcessedFiles;

    QList<QPair<QString, QString> > mFileDiffs = splitDiff(sFullDiff);

    for (QPair<QString, QString> tPair : mFileDiffs)
    {
        QString sFileName = tPair.first;
        QString sFileDiff = tPair.second;

        // Read the file, apply the diff and write to merge directory
        QString sFullSourceName = m_pDatabase->composeLocalFileName(sFileName);
        QString sFullTargetName = m_pDatabase->composeMergeFileName(sFileName);

        // Check if file is already present in merge directory
        bool bMergedFileExists = QFile(sFullTargetName).exists();

        if (bMergedFileExists)
            sFullSourceName = sFullTargetName;

        // Get the content of the file
        QString sPreviousContent = CUtils::getTextFileContent(sFullSourceName);
        QString sNewContent = CUtils::applyUnifiedDiff(sPreviousContent, sFileDiff);

        if (sPreviousContent.isEmpty() && sNewContent.isEmpty())
        {
            OUT_ERROR(QString(CStrings::s_sTextCouldNotApplyPatch).arg(sFileName));
            OUT_INFO(sFileDiff);
            return false;
        }

        CUtils::putTextFileContent(sFullTargetName, sNewContent);
        QString sId = CUtils::idFromByteArray(sNewContent.toUtf8());
        mProcessedFiles[sFileName] = sId;
    }

    // Stage the merged files
    if (bAddToStage && IS_NOT_NULL(m_pStagingCommit))
    {
        for (QString sFileName : mProcessedFiles.values())
        {
            QString sKey = mapKeyForValue(mProcessedFiles, sFileName);
            m_pStagingCommit->addFile(mProcessedFiles[sKey], sFileName);
        }
    }

    // Move the merged files
    for (QString sFileName : mProcessedFiles.keys())
    {
        QString sFullSourceName = m_pDatabase->composeMergeFileName(sFileName);
        QString sFullTargetName = m_pDatabase->composeLocalFileName(sFileName);

        if (not CUtils::moveFile(sFullSourceName, sFullTargetName))
        {
            OUT_ERROR(QString("Could not move file %1.").arg(sFullSourceName));
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

QList<CFile> CRepository::fileStatus(const QStringList& lFileNames)
{
    m_eStatus = checkStatus(lFileNames);
    return m_lFiles;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readGeneralInfo()
{
    QString sFileName = QString("%1/%2")
            .arg(m_pDatabase->dataPath())
            .arg(CStrings::s_sGeneralInfoFileName);

    CXMLNode xInfo = CXMLNode::load(sFileName);

    CXMLNode xBranches = xInfo.getNodeByTagName(CStrings::s_sParamBranches);
    setCurrentBranchName(xBranches.attributes()[CStrings::s_sParamCurrent]);

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

bool CRepository::writeGeneralInfo()
{
    QString sFileName = QString("%1/%2")
            .arg(m_pDatabase->dataPath())
            .arg(CStrings::s_sGeneralInfoFileName);

    CXMLNode xInfo(CStrings::s_sParamInfo);

    CXMLNode xBranches(CStrings::s_sParamBranches);
    xBranches.attributes()[CStrings::s_sParamCurrent] = m_sCurrentBranchName;
    xInfo << xBranches;

    xInfo.save(sFileName);

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

}
