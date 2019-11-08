
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CDatabase.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CDatabase::CDatabase(const QString& sRootPath, QObject* parent)
    : QObject(parent)
    , m_sRootPath(sRootPath)
{
    if (m_sRootPath.endsWith(CStrings::s_sPathSep))
        m_sRootPath.remove(m_sRootPath.count() - 1, 1);

    m_sDataPath = QString("%1/%2").arg(m_sRootPath).arg(CStrings::s_sPathAzeDataRoot);
    m_sStashPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeStashPath);
    m_sMergePath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeMergePath);
    m_sBranchPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeBranchPath);
    m_sCommitPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeCommitPath);
    m_sObjectPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeObjectPath);
}

//-------------------------------------------------------------------------------------------------

CBranch* CDatabase::getBranch(const QString& sName, QObject* parent)
{
    return CBranch::fromFile(composeBranchFileName(sName), parent);
}

//-------------------------------------------------------------------------------------------------

CCommit* CDatabase::getCommit(const QString& sId, QObject* parent)
{
    return CCommit::fromFile(composeCommitFileName(sId), parent, sId);
}

//-------------------------------------------------------------------------------------------------

QByteArray CDatabase::getObject(const QString& sId)
{
    QByteArray baData;

    QFile tOutputFile(composeObjectFileName(sId));

    // Does the object file exist?
    if (tOutputFile.exists())
    {
        if (tOutputFile.open(QIODevice::ReadOnly))
        {
            // Uncompress object file data
            baData = qUncompress(tOutputFile.readAll());
            tOutputFile.close();
        }
    }

    return baData;
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::storeCommit(const CCommit* pCommit, const QString& sId)
{
    if (IS_NULL(pCommit))
        return "";

    QString sIdToUse = sId;

    if (sIdToUse.isEmpty())
        sIdToUse = pCommit->generateId();

    pCommit->toFile(composeCommitFileName(sIdToUse));

    return sIdToUse;
}

//-------------------------------------------------------------------------------------------------

bool CDatabase::init()
{
    QDir rootDir(m_sRootPath);

    if (not rootDir.exists())
        return false;

    rootDir.mkpath(m_sDataPath);
    rootDir.mkpath(m_sStashPath);
    rootDir.mkpath(m_sMergePath);
    rootDir.mkpath(m_sBranchPath);
    rootDir.mkpath(m_sCommitPath);
    rootDir.mkpath(m_sObjectPath);

    return true;
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::relativeFileName(const QString& sFileName)
{
    return CUtils::relativeFileName(m_sRootPath, sFileName);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::absoluteFileName(const QString& sFileName)
{
    return CUtils::absoluteFileName(m_sRootPath, sFileName);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::composeLocalFileName(const QString& sFileName)
{
    return QString("%1/%2").arg(m_sRootPath).arg(sFileName);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::composeMergeFileName(const QString& sFileName)
{
    return QString("%1/%2").arg(m_sMergePath).arg(sFileName);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::composeBranchFileName(const QString& sBranchName)
{
    return QString("%1/%2.%3")
            .arg(m_sBranchPath)
            .arg(sBranchName)
            .arg(CStrings::s_sCompressedXMLExtension);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::composeCommitFileName(const QString& sCommitId)
{
    return QString("%1/%2.%3")
            .arg(m_sCommitPath)
            .arg(sCommitId)
            .arg(CStrings::s_sCompressedXMLExtension);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::composeObjectFileName(const QString& sId)
{
    return QString("%1/%2")
            .arg(m_sObjectPath)
            .arg(sId);
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::storeFile(const QString& sFileName)
{
    QString sId;
    QFile tInputFile(sFileName);

    if (tInputFile.exists())
    {
        if (tInputFile.open(QIODevice::ReadOnly))
        {
            QByteArray baData = tInputFile.readAll();
            tInputFile.close();

            // Get the Id of this file
            sId = CUtils::idFromByteArray(baData);

            // Check if it already exists
            QString sStoredObjectFileName = QString("%1/%2").arg(m_sObjectPath).arg(sId);
            QFile tOutputFile(sStoredObjectFileName);

            // If it does not exist, compress it and store it in objects folder
            if (not tOutputFile.exists())
            {
                baData = qCompress(baData);

                if (tOutputFile.open(QIODevice::WriteOnly))
                {
                    if (tOutputFile.write(baData) != baData.count())
                    {
                        sId = "";
                        tOutputFile.close();
                        tOutputFile.remove();
                    }
                    else
                    {
                        tOutputFile.close();
                    }
                }
            }
        }
    }

    return sId;
}

//-------------------------------------------------------------------------------------------------

bool CDatabase::fileWithIdExists(const QString& sId)
{
    QString sStoredObjectFileName = QString("%1/%2").arg(m_sObjectPath).arg(sId);
    return QFile(sStoredObjectFileName).exists();
}

//-------------------------------------------------------------------------------------------------

QString CDatabase::printableFileContentById(const QString& sId)
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
        return QString(getObject(sId));

    return sText;
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
