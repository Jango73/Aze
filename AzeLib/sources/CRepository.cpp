
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

    branch(CStrings::s_sDefaultBranchName);

    writeGeneralInfo();
    writeCurrentBranch();

    setOk(true);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::branch(const QString& sName)
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
        setCurrentBranchName(sName);
    }

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
    Q_UNUSED(sAuthor);
    Q_UNUSED(sMessage);

    // Check presence of current branch
    if (IS_NULL(m_pCurrentBranch))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    // Check presence and relevancy of staging commmit
    if (IS_NULL(m_pStagingCommit) || m_pStagingCommit->files().count() == 0)
    {
        OUT_ERROR(CStrings::s_sTextCommitEmpty);
        return false;
    }

    OUT_DEBUG("Stage commit ok");

    CCommit* pNewCommit = nullptr;

    if (readTipCommit())
    {
        pNewCommit = m_pTipCommit->clone();
    }
    else
    {
        pNewCommit = new CCommit();
    }

    OUT_DEBUG(QString("Tip commit id=%1").arg(pNewCommit->id()));

    pNewCommit->add(m_sRootPath, m_sObjectPath, m_pStagingCommit);

    QString sCommitContent = pNewCommit->toNode().toString();
    QString sCommitId = CUtils::idFromString(sCommitContent);
    QString sCommitFileName = QString("%1/%2.%3")
            .arg(m_sCommitPath)
            .arg(sCommitId)
            .arg(CStrings::s_sCompressedXMLExtension);

    pNewCommit->toFile(sCommitFileName);

    // Update the current branch
    m_pCurrentBranch->setTipCommitId(sCommitId);

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
    QString sFileName = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sGeneralInfoFileName);

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
            QString sTipFileName = QString("%1/%2.%3")
                    .arg(m_sCommitPath)
                    .arg(m_pCurrentBranch->tipCommitId())
                    .arg(CStrings::s_sCompressedXMLExtension);

            setTipCommit(CCommit::fromFile(sTipFileName));
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::writeGeneralInfo()
{
    QString sFileName = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sGeneralInfoFileName);

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
    QString sFileName = QString("%1/%2.%3").arg(m_sBranchPath).arg(m_sCurrentBranchName).arg(CStrings::s_sCompressedXMLExtension);

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
