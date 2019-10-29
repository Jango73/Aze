
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRepository.h"
#include "CUtils.h"

#include "commands/CStageCommand.h"
#include "commands/CUnstageCommand.h"
#include "commands/CRemoveCommand.h"
#include "commands/CCommitCommand.h"
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
        switchToBranch(CStrings::s_sDefaultBranchName);

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
    CBranch* pNewBranch = new CBranch(this);

    if (not IS_NULL(m_pCurrentBranch))
    {
        pNewBranch->setRootCommitId(m_pCurrentBranch->tipCommitId());
        pNewBranch->setTipCommitId(m_pCurrentBranch->tipCommitId());
    }

    pNewBranch->toNode().save(m_pDatabase->composeBranchFileName(sName));

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::switchToBranch(const QString& sName)
{
    QString sFileName = m_pDatabase->composeBranchFileName(sName);

    if (QFile(sFileName).exists())
    {
        setCurrentBranch(CBranch::fromNode(CXMLNode::load(sFileName), this));
        setCurrentBranchName(sName);

        return true;
    }

    return false;
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

QString CRepository::log(const QStringList& lFileNames)
{
    QString sReturnValue;

    if (CLogCommand(this, lFileNames, &sReturnValue).execute())
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

QList<CFile> CRepository::fileStatus(const QStringList& lFileNames)
{
    QList<CFile> lReturnValue;

    CStatusCommand(this, lFileNames, &lReturnValue).execute();

    return lReturnValue;
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
        if (IS_NULL(m_pRootCommit))
        {
            setRootCommit(m_pDatabase->getCommit(m_pCurrentBranch->rootCommitId(), this));
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::readTipCommit()
{
    if (not m_pCurrentBranch->tipCommitId().isEmpty())
    {
        if (IS_NULL(m_pTipCommit))
        {
            setTipCommit(m_pDatabase->getCommit(m_pCurrentBranch->tipCommitId(), this));
            return true;
        }
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

CCommit* CRepository::getCommitAncestor(CCommit* pCommit, QObject* parent, int iDelta)
{
    CCommit* pAncestor = pCommit;
    int iGuard = 999999;

    while (true)
    {
        QList<CCommit*> parents = CCommit::parentList(m_pDatabase, pAncestor, this);

        if (parents.count() == 0)
        {
            qDeleteAll(parents);
            pAncestor = nullptr;
            break;
        }

        // The first parent is the one to follow in order to stay on branch of pCommit
        SAFE_DELETE(pAncestor);
        pAncestor = parents[0]->clone(parent);
        iDelta--;
        iGuard--;

        qDeleteAll(parents);

        if (iDelta == 0 || iGuard == 0)
            return pAncestor;
    }

    return pAncestor;
}

//-------------------------------------------------------------------------------------------------

QList<CCommit*> CRepository::getCommitAncestorList(CCommit* pCommit, QObject* parent, int iMaxCount)
{
    QList<CCommit*> lReturnValue;
    CCommit* pAncestor = pCommit;
    int iGuard = iMaxCount == 0 ? 999999 : iMaxCount;

    while (true)
    {
        QList<CCommit*> parents = CCommit::parentList(m_pDatabase, pAncestor, this);

        if (parents.count() == 0)
        {
            qDeleteAll(parents);
            break;
        }

        // The first parent is the one to follow in order to stay on branch of pCommit
        pAncestor = parents[0]->clone(parent);
        lReturnValue << pAncestor;
        iGuard--;

        qDeleteAll(parents);

        if (iGuard == 0)
            break;
    }

    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

CCommit* CRepository::getCommitsCommonAncestor(CCommit* pCommit1, CCommit* pCommit2, QObject* parent)
{
    CCommit* pAncestor = nullptr;
    QList<CCommit*> pAncestors1 = getCommitAncestorList(pCommit1, parent);
    QList<CCommit*> pAncestors2 = getCommitAncestorList(pCommit2, parent);

    for (CCommit* pCommit1 : pAncestors1)
    {
        QString sId = pCommit1->id();

        for (CCommit* pCommit2 : pAncestors2)
        {
            if (sId == pCommit2->id())
            {
                pAncestor = pCommit1->clone(parent);
                break;
            }
        }
    }

    qDeleteAll(pAncestors1);
    qDeleteAll(pAncestors2);

    return pAncestor;
}

//-------------------------------------------------------------------------------------------------

CCommit* CRepository::workingDirectoryAsCommit(QObject* parent)
{
    CCommit* pNewCommit = new CCommit(parent);
    QStringList lFiles;

    listFilesRecursive(lFiles, m_pDatabase->rootPath(), ".");

    for (QString sFile : lFiles)
        pNewCommit->addFile(sFile);

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
        lStack << m_pDatabase->relativeFileName(sFullName);
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
