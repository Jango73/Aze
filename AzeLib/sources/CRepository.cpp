
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
    , m_pStagingCommit(nullptr)
{
    m_sDataPath = QString("%1/%2").arg(m_sRootPath).arg(CStrings::s_sPathAzeDataRoot);
    m_sBranchPath = QString("%1/%2").arg(CStrings::s_sPathAzeDataRoot).arg(CStrings::s_sPathAzeBranchPath);
    m_sCommitPath = QString("%1/%2").arg(CStrings::s_sPathAzeDataRoot).arg(CStrings::s_sPathAzeCommitPath);
    m_sObjectPath = QString("%1/%2").arg(CStrings::s_sPathAzeDataRoot).arg(CStrings::s_sPathAzeObjectPath);

    // Check if repository is ok
    QDir dataPath(m_sDataPath);
    if (dataPath.exists())
        setOk(true);
}

//-------------------------------------------------------------------------------------------------

CRepository::~CRepository()
{
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

    setOk(true);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::add(const QStringList& lRelativeFileName)
{
    for (QString sFile : lRelativeFileName)
    {
        if (not addSingleFile(sFile))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::remove(const QStringList& lRelativeFileName)
{
    for (QString sFile : lRelativeFileName)
    {
        if (not removeSingleFile(sFile))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

QList<CFile> CRepository::fileStatus()
{
    QList<CFile> lReturnValue;

    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CRepository::readStage()
{
    QString sStagingCommitFileName = QString("%1/%2").arg(m_sCommitPath).arg(CStrings::s_sStagingCommitFileName);
    setStagingCommit(CCommit::fromFile(sStagingCommitFileName));
}

//-------------------------------------------------------------------------------------------------

QString CRepository::absoluteFileName(const QString& sFileName)
{
    QFileInfo info(sFileName);

    if (info.isAbsolute())
        return sFileName;

    return QString("%1/%2").arg(m_sRootPath).arg(sFileName);
}

//-------------------------------------------------------------------------------------------------

bool CRepository::addSingleFile(QString sFileName)
{
    QString sAbsoluteFileName = absoluteFileName(sFileName);
    QFile file(sAbsoluteFileName);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRepository::removeSingleFile(QString sFileName)
{
    QString sAbsoluteFileName = absoluteFileName(sFileName);
    QFile file(sAbsoluteFileName);

    return true;
}

}
