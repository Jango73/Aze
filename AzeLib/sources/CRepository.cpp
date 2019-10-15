
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRepository.h"
#include "CUtils.h"

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
    m_sBranchPath = QString("%1/%2").arg(CStrings::s_sPathAzeDataRoot).arg(CStrings::s_sPathAzeBranchPath);
    m_sCommitPath = QString("%1/%2").arg(CStrings::s_sPathAzeDataRoot).arg(CStrings::s_sPathAzeCommitPath);
    m_sObjectPath = QString("%1/%2").arg(CStrings::s_sPathAzeDataRoot).arg(CStrings::s_sPathAzeObjectPath);

    m_sStagingCommitFileName = QString("%1/%2").arg(m_sCommitPath).arg(CStrings::s_sStagingCommitFileName);

    // Check if repository is ok
    QDir dataPath(m_sDataPath);
    if (dataPath.exists())
        setOk(true);

    if (m_bOk)
    {
        readGeneralInfo();
        readCurrentBranch();
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
    for (QString sFileName : lFileNames)
    {
        QString sRelativeFileName = CUtils::relativeFileName(m_sRootPath, sFileName);

        OUT_DEBUG(sRelativeFileName);

        if (not addSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::remove(const QStringList& lFileNames)
{
    for (QString sFileName : lFileNames)
    {
        QString sRelativeFileName = CUtils::relativeFileName(m_sRootPath, sFileName);

        OUT_DEBUG(sRelativeFileName);

        if (not removeSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::commit(const QString& sAuthor, const QString& sMessage)
{
    OUT_DEBUG(QString("sAuthor=%1").arg(sAuthor));
    OUT_DEBUG(QString("sMessage=%1").arg(sMessage));

    // Check presence of current branch
    if (IS_NULL(m_pCurrentBranch))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    OUT_DEBUG(QString("current branch=%1").arg(m_sCurrentBranchName));

    // Check presence and relevancy of staging commmit
    if (IS_NULL(m_pStagingCommit) || m_pStagingCommit->files().count() == 0)
    {
        OUT_ERROR(CStrings::s_sTextCommitEmpty);
        return false;
    }

    // Create a new commit object
    CCommit* pNewCommit = nullptr;

    // Read the tip commit of the current branch
    if (readTipCommit())
    {
        OUT_DEBUG(QString("Existing tip commit id=%1").arg(m_pTipCommit->id()));

        pNewCommit = m_pTipCommit->clone();
        pNewCommit->addParent(m_pTipCommit->id());
    }
    else
    {
        OUT_DEBUG("No current tip commit");

        pNewCommit = new CCommit();
    }

    pNewCommit->addCommit(m_sRootPath, m_sObjectPath, m_pStagingCommit);

    QString sNewCommitContent = pNewCommit->toNode().toString();
    QString sNewCommitId = CUtils::idFromString(sNewCommitContent);
    QString sNewCommitFileName = composeCommitFileName(sNewCommitId);

    // Finalize and save the commit
    pNewCommit->setId(sNewCommitId);
    pNewCommit->setDate(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    pNewCommit->setAuthor(sAuthor);
    pNewCommit->setMessage(sMessage);
    pNewCommit->toFile(sNewCommitFileName);

    // Update the current branch
    m_pCurrentBranch->setTipCommitId(sNewCommitId);

    delete pNewCommit;

    return true;
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
    QString sFileName = QString("%1/%2.%3")
            .arg(m_sBranchPath)
            .arg(m_sCurrentBranchName)
            .arg(CStrings::s_sCompressedXMLExtension);

    setCurrentBranch(CBranch::fromNode(CXMLNode::load(sFileName)));

    OUT_DEBUG(QString("Root: %1").arg(m_pCurrentBranch->rootCommitId()));
    OUT_DEBUG(QString("Tip: %1").arg(m_pCurrentBranch->tipCommitId()));

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
            OUT_DEBUG(QString("Reading commit %1").arg(sTipFileName));
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
    QString sFileName = QString("%1/%2.%3")
            .arg(m_sBranchPath)
            .arg(m_sCurrentBranchName)
            .arg(CStrings::s_sCompressedXMLExtension);

    if (m_pCurrentBranch != nullptr)
    {
        m_pCurrentBranch->toNode().save(sFileName);
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

QString CRepository::getFileContent(const QString& sId)
{
    QString sText;
    QString sFileName;

    // Try in commits
    sFileName = composeCommitFileName(sId);

    if (QFile(sFileName).exists())
        return CUtils::getFileContent(sFileName);

    // Try in objects
    sFileName = composeObjectFileName(sId);

    if (QFile(sFileName).exists())
        return CUtils::getFileFromDB(m_sObjectPath, sId);

    return sText;
}

//-------------------------------------------------------------------------------------------------

QString CRepository::composeCommitFileName(const QString& sId)
{
    return QString("%1/%2.%3")
            .arg(m_sCommitPath)
            .arg(sId)
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

bool CRepository::addSingleFile(QString sRelativeFileName)
{
    OUT_DEBUG(sRelativeFileName);

    if (not IS_NULL(m_pStagingCommit))
    {
        if (CUtils::fileExists(m_sRootPath, sRelativeFileName))
        {
            QString sId = CUtils::idFromString(sRelativeFileName);
            m_pStagingCommit->addFile(sRelativeFileName, sId);
            return true;
        }
        else
        {
            OUT_ERROR(QString("%1: %2").arg(CStrings::s_sTextNoSuchFile).arg(sRelativeFileName));
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::removeSingleFile(QString sRelativeFileName)
{
    OUT_DEBUG(sRelativeFileName);

    return true;
}

}
