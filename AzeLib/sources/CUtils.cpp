
// Qt
#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QDataStream>
#include <QCryptographicHash>

// Application
#include "CUtils.h"
#include "CStrings.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

QString CUtils::relativeFileName(const QString& sRootPath, const QString& sFileName)
{
    QDir dRoot(sRootPath);
    return dRoot.relativeFilePath(sFileName);
}

//-------------------------------------------------------------------------------------------------

QString CUtils::absoluteFileName(const QString& sRootPath, const QString& sFileName)
{
    QFileInfo info(sFileName);

    if (info.isAbsolute())
        return sFileName;

    return QString("%1/%2").arg(sRootPath).arg(sFileName);
}

//-------------------------------------------------------------------------------------------------

bool CUtils::fileExists(const QString& sRootPath, const QString& sRelativeFileName)
{
    QString sAbsoluteName = CUtils::absoluteFileName(sRootPath, sRelativeFileName);
    return QFile(sAbsoluteName).exists();
}

//-------------------------------------------------------------------------------------------------

QDictionary CUtils::dictionaryFromNode(const CXMLNode& xNode)
{
    QDictionary mValue;

    for (CXMLNode xKey : xNode.nodes())
        mValue[xKey.tag()] = xKey.value();

    return mValue;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CUtils::nodeFromDictionary(const QDictionary& xDict)
{
    CXMLNode xValue;

    for (QString sKey : xDict.keys())
    {
        CXMLNode xItem(sKey);
        xItem.setValue(xDict[sKey]);
        xValue << xItem;
    }

    return xValue;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::serializeBase64(const QString& sText)
{
    return QString(CStrings::s_sBase64Marker) + QString(sText.toUtf8().toBase64());
}

//-------------------------------------------------------------------------------------------------

QString CUtils::deserializeBase64(const QString& sText)
{
    QString sInput = sText;
    if (sInput.startsWith(CStrings::s_sBase64Marker))
    {
        sInput.remove(0, CStrings::s_sBase64Marker.length());
        return QString(QByteArray::fromBase64(sInput.toLatin1()));
    }
    return sInput;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::idFromByteArray(const QByteArray& baData)
{
    QCryptographicHash crypto(QCryptographicHash::Sha1);
    crypto.addData(baData);
    QString sResult = QString(crypto.result().toHex());
    return sResult;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::idFromString(const QString& sText)
{
    return idFromByteArray(sText.toUtf8());
}

//-------------------------------------------------------------------------------------------------

QString CUtils::idFromFile(const QString& sFilename)
{
    QString sId;
    QFile file(sFilename);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray baData = file.readAll();
        file.close();
        sId = idFromByteArray(baData);
    }
    return sId;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::packIdAndFile(const QString& sId, const QString& sFilePath)
{
    return QString("%1%2%3").arg(sId).arg(CStrings::s_sPathIdSeparator).arg(sFilePath);
}

//-------------------------------------------------------------------------------------------------

void CUtils::unpackIdAndFile(const QString& sPack, QString& sId, QString& sFilePath)
{
    QStringList sValues = sPack.split(CStrings::s_sPathIdSeparator);
    if (sValues.count() == 2)
    {
        sId = sValues[0];
        sFilePath = sValues[1];
    }
}

//-------------------------------------------------------------------------------------------------

QByteArray CUtils::getBinaryFileContent(const QString& sFileName)
{
    QByteArray baData;
    QFile tInputFile(sFileName);

    if (tInputFile.exists())
    {
        if (tInputFile.open(QIODevice::ReadOnly))
        {
            baData = tInputFile.readAll();
            tInputFile.close();
        }
    }

    return baData;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::getTextFileContent(const QString& sFileName)
{
    return QString(getBinaryFileContent(sFileName));
}

//-------------------------------------------------------------------------------------------------

bool CUtils::fileExistsInDB(const QString& sObjectPath, const QString& sId)
{
    QString sStoredObjectFileName = QString("%1/%2").arg(sObjectPath).arg(sId);
    return QFile(sStoredObjectFileName).exists();
}

//-------------------------------------------------------------------------------------------------

QByteArray CUtils::getFileFromDB(const QString& sObjectPath, const QString& sId)
{
    QByteArray baData;

    QString sStoredObjectFileName = QString("%1/%2").arg(sObjectPath).arg(sId);
    QFile tOutputFile(sStoredObjectFileName);

    if (tOutputFile.exists())
    {
        if (tOutputFile.open(QIODevice::ReadOnly))
        {
            baData = qUncompress(tOutputFile.readAll());
            tOutputFile.close();
        }
    }

    return baData;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::storeFileInDB(const QString& sObjectPath, const QString& sFileName)
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
            sId = idFromByteArray(baData);

            // Check if it already exists
            QString sStoredObjectFileName = QString("%1/%2").arg(sObjectPath).arg(sId);
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

std::vector<std::string> CUtils::textToStdStringVector(const QString& sText)
{
    std::vector<std::string> vReturnValue;
    QStringList lLines = sText.split(CStrings::s_sNewLine);
    for (QString sLine: lLines)
        vReturnValue.push_back(sLine.toUtf8().constData());
    return vReturnValue;
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
