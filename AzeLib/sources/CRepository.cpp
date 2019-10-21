
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRepository.h"
#include "CUtils.h"

#include "commands/CAddCommand.h"
#include "commands/CRemoveCommand.h"
#include "commands/CCommitCommand.h"
#include "commands/CDiffCommand.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRepository::CRepository(const QString& sRootPath, QObject* parent)
    : CObject(parent)
    , m_bOk(false)
    , m_sRootPath(sRootPath)
    , m_pCurrentBranch(nullptr)
    , m_pStagingCommit(nullptr)
    , m_pRootCommit(nullptr)
    , m_pTipCommit(nullptr)
{
    m_sDataPath = QString("%1/%2").arg(m_sRootPath).arg(CStrings::s_sPathAzeDataRoot);
    m_sBranchPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeBranchPath);
    m_sCommitPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeCommitPath);
    m_sObjectPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeObjectPath);
    m_sStagingCommitFileName = composeCommitFileName(CStrings::s_sStagingCommitFileName);

    OUT_DEBUG(QString("m_sDataPath: %1").arg(m_sDataPath));
    OUT_DEBUG(QString("m_sBranchPath: %1").arg(m_sBranchPath));
    OUT_DEBUG(QString("m_sCommitPath: %1").arg(m_sCommitPath));
    OUT_DEBUG(QString("m_sObjectPath: %1").arg(m_sObjectPath));
    OUT_DEBUG(QString("m_sStagingCommitFileName: %1").arg(m_sStagingCommitFileName));

    // Check if repository is ok
    QDir dataPath(m_sDataPath);
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
    SAFE_DELETE(m_pCurrentBranch);
    SAFE_DELETE(m_pStagingCommit);
    SAFE_DELETE(m_pRootCommit);
    SAFE_DELETE(m_pTipCommit);
}

//-------------------------------------------------------------------------------------------------

bool CRepository::init()
{
    QDir rootDir(m_sRootPath);

    if (not rootDir.exists())
        return false;

    qDebug() << "Creating repository data...";

    rootDir.mkpath(m_sDataPath);
    rootDir.mkpath(m_sBranchPath);
    rootDir.mkpath(m_sCommitPath);
    rootDir.mkpath(m_sObjectPath);

    createBranch(CStrings::s_sDefaultBranchName);

    writeGeneralInfo();
    writeCurrentBranch();

    setOk(true);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::createBranch(const QString& sName)
{
    if (not IS_NULL(m_pCurrentBranch))
    {
        m_pCurrentBranch->setRootCommitId(m_pCurrentBranch->tipCommitId());
        m_pCurrentBranch->setType(CEnums::eBranch);
    }
    else
    {
        CBranch* pNewBranch = new CBranch();
        setCurrentBranch(pNewBranch);
    }

    setCurrentBranchName(sName);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::add(const QStringList& lFileNames)
{
    return CAddCommand(this, lFileNames).execute();
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

QList<CFile> CRepository::fileStatus()
{
    QList<CFile> lReturnValue;

    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readGeneralInfo()
{
    QString sFileName = QString("%1/%2")
            .arg(m_sDataPath)
            .arg(CStrings::s_sGeneralInfoFileName);

    CXMLNode xInfo = CXMLNode::load(sFileName);

    CXMLNode xBranches = xInfo.getNodeByTagName(CStrings::s_sParamBranches);
    setCurrentBranchName(xBranches.attributes()[CStrings::s_sParamCurrent]);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readCurrentBranch()
{
    QString sFileName = composeBranchFileName(m_sCurrentBranchName);
    setCurrentBranch(CBranch::fromNode(CXMLNode::load(sFileName)));

    OUT_DEBUG(QString("Current branch root: %1").arg(m_pCurrentBranch->rootCommitId()));
    OUT_DEBUG(QString("Current branch tip: %1").arg(m_pCurrentBranch->tipCommitId()));

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readStage()
{
    if (IS_NULL(m_pStagingCommit))
    {
        setStagingCommit(CCommit::fromFile(m_sStagingCommitFileName));
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readTipCommit()
{
    if (not m_pCurrentBranch->tipCommitId().isEmpty())
    {
        if (IS_NULL(m_pTipCommit))
        {
            QString sTipId = m_pCurrentBranch->tipCommitId();
            QString sTipFileName = composeCommitFileName(sTipId);
            setTipCommit(CCommit::fromFile(sTipFileName));
            m_pTipCommit->setId(sTipId);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::writeGeneralInfo()
{
    QString sFileName = QString("%1/%2")
            .arg(m_sDataPath)
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
    {
        m_pCurrentBranch->toNode().save(composeBranchFileName(m_sCurrentBranchName));
    }

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
        m_pStagingCommit = new CCommit();

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

QString CRepository::getFileContentFromId(const QString& sId)
{
    QString sText;
    QString sFileName;

    // Try in commits
    sFileName = composeCommitFileName(sId);

    if (QFile(sFileName).exists())
        return CUtils::getTextFileContent(sFileName);

    // Try in objects
    sFileName = composeObjectFileName(sId);

    if (QFile(sFileName).exists())
        return CUtils::getFileFromDB(m_sObjectPath, sId);

    return sText;
}

//-------------------------------------------------------------------------------------------------

QString CRepository::getFileContentFromFileName(const QString& sFileName)
{
    return CUtils::getTextFileContent(sFileName);
}

//-------------------------------------------------------------------------------------------------

QString CRepository::composeBranchFileName(const QString& sBranchName)
{
    if (m_sCurrentBranchName.isEmpty())
        return "";

    return QString("%1/%2.%3")
            .arg(m_sBranchPath)
            .arg(sBranchName)
            .arg(CStrings::s_sCompressedXMLExtension);
}

//-------------------------------------------------------------------------------------------------

QString CRepository::composeCommitFileName(const QString& sCommitId)
{
    return QString("%1/%2.%3")
            .arg(m_sCommitPath)
            .arg(sCommitId)
            .arg(CStrings::s_sCompressedXMLExtension);
}

//-------------------------------------------------------------------------------------------------

QString CRepository::composeObjectFileName(const QString& sId)
{
    return QString("%1/%2")
            .arg(m_sObjectPath)
            .arg(sId);
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
    if (sText.contains("-"))
    {
        QStringList lValues = sText.split("-");

        if (lValues.count() > 1)
            iDelta = lValues[1].toInt() * -1;

        return lValues[0];
    }

    return sText;
}

//-------------------------------------------------------------------------------------------------

CCommit* CRepository::workingDirectoryAsCommit()
{
    CCommit* pNewCommit = new CCommit();
    QStringList lFiles;

    listFilesRecursive(lFiles, m_sRootPath, ".");

    for (QString sFile : lFiles)
    {
        pNewCommit->addFile(sFile);
    }

    return pNewCommit;
}

//-------------------------------------------------------------------------------------------------

void CRepository::listFilesRecursive(QStringList& lStack, QString sRootDirectory, QString sCurrentDirectory)
{
    QStringList lFilter; lFilter << "*.*";

    QDir dDirectory(sCurrentDirectory);
    QFileInfoList lFiles = dDirectory.entryInfoList(lFilter, QDir::Files | QDir::NoSymLinks);

    for (QFileInfo iFile : lFiles)
    {
        QString sFullName = QString("%1/%2").arg(iFile.absolutePath()).arg(iFile.fileName());
        lStack << CUtils::relativeFileName(sRootDirectory, sFullName);
    }

    lFiles = dDirectory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    for (QFileInfo iFile : lFiles)
    {
        QString sTargetDirectory = QString("%1/%2").arg(sCurrentDirectory).arg(iFile.fileName());
        listFilesRecursive(lStack, sRootDirectory, sTargetDirectory);
    }
}

//-------------------------------------------------------------------------------------------------

}
