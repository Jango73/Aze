
// std
#include <string>
#include <sstream>

// dtl
#include <dtl.hpp>

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

using dtl::Diff;

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

std::vector<std::string> CUtils::textToStdStringVector(const QString& sText)
{
    std::vector<std::string> vReturnValue;
    QStringList lLines = sText.split(CStrings::s_sNewLine);
    for (QString sLine: lLines)
        vReturnValue.push_back(sLine.toUtf8().constData());
    return vReturnValue;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::unifiedDiff(const QString& sText1, const QString& sText2)
{
    std::vector<std::string> lText1 = CUtils::textToStdStringVector(sText1);
    std::vector<std::string> lText2 = CUtils::textToStdStringVector(sText2);

    std::stringstream strStream;

    Diff<std::string> diff(lText1, lText2);
    diff.onHuge();
    diff.compose();
    diff.composeUnifiedHunks();
    diff.printUnifiedFormat(strStream);

    return QString::fromUtf8(strStream.str().data());
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
