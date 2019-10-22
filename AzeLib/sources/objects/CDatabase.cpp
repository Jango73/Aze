
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
    m_sDataPath = QString("%1/%2").arg(m_sRootPath).arg(CStrings::s_sPathAzeDataRoot);
    m_sBranchPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeBranchPath);
    m_sCommitPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeCommitPath);
    m_sObjectPath = QString("%1/%2").arg(m_sDataPath).arg(CStrings::s_sPathAzeObjectPath);

    OUT_DEBUG(QString("m_sDataPath: %1").arg(m_sDataPath));
    OUT_DEBUG(QString("m_sBranchPath: %1").arg(m_sBranchPath));
    OUT_DEBUG(QString("m_sCommitPath: %1").arg(m_sCommitPath));
    OUT_DEBUG(QString("m_sObjectPath: %1").arg(m_sObjectPath));
}

//-------------------------------------------------------------------------------------------------

bool CDatabase::init()
{
    QDir rootDir(m_sRootPath);

    if (not rootDir.exists())
        return false;

    rootDir.mkpath(m_sDataPath);
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
                OUT_DEBUG(QString("Creating : %1").arg(sStoredObjectFileName));

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

QByteArray CDatabase::fileContent(const QString& sId)
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
        return QString(fileContent(sId));

    return sText;
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
