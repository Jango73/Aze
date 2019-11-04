
// std
//#include <string>
//#include <sstream>

// Qt
#include <QString>
#include <QList>
#include <QMap>
#include <QPair>
#include <QVariant>
#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QDataStream>
#include <QCryptographicHash>

// diff-match-patch
// https://github.com/google/diff-match-patch
#include "diff_match_patch.h"

// Application
#include "CUtils.h"
#include "CStrings.h"

namespace Aze {

//using dtl::Diff;

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

QString CUtils::idFromFileContent(const QString& sFilename)
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

bool CUtils::idValid(const QString& sId)
{
    // In hex strings, there are 2 characters per byte
    return (sId.count() / 2) == QCryptographicHash::hashLength(QCryptographicHash::Sha1);
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

bool CUtils::putBinaryFileContent(const QString& sFileName, const QByteArray& baData)
{
    QFile tInputFile(sFileName);

    if (tInputFile.open(QIODevice::WriteOnly))
    {
        tInputFile.write(baData);
        tInputFile.close();

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CUtils::putTextFileContent(const QString& sFileName, const QString& sContent)
{
    return putBinaryFileContent(sFileName, sContent.toUtf8());
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

QString CUtils::fileDiffHeader(const QString& sLeft, const QString& sRight)
{
    QString sOutput;

    sOutput += QString("%1 %2 %3%4")
            .arg(CStrings::s_sDiffChunkHeader)
            .arg(sLeft)
            .arg(sRight)
            .arg(CStrings::s_sNewLine);

    sOutput += QString("--- %1%2")
            .arg(sLeft)
            .arg(CStrings::s_sNewLine);

    sOutput += QString("+++ %1%2")
            .arg(sRight)
            .arg(CStrings::s_sNewLine);

    return sOutput;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::unifiedDiff(const QString& sText1, const QString& sText2)
{
//    std::wstring wText1 = sText1.toStdWString();
//    std::wstring wText2 = sText2.toStdWString();
//    diff_match_patch<std::wstring> dmp;
//    return QString::fromStdWString(dmp.patch_toText(dmp.patch_make(wText1, wText2)));
    diff_match_patch diff;
    return diff.patch_toText(diff.patch_make(sText1, sText2));
}

//-------------------------------------------------------------------------------------------------

QString CUtils::applyUnifiedDiff(const QString& sText, const QString& sDiff)
{
//    std::wstring wText1 = sText.toStdWString();
//    std::wstring wText2 = sDiff.toStdWString();
//    diff_match_patch<std::wstring> dmp;
//    std::pair<std::wstring, std::vector<bool> > out = dmp.patch_apply(dmp.patch_fromText(wText2), wText1);
//    return QString::fromStdWString(out.first);
    diff_match_patch diff;
    QList<Patch> patches = diff.patch_fromText(sDiff);
    return diff.patch_apply(patches, sText).first;
}

//-------------------------------------------------------------------------------------------------

QString CUtils::printableUnifiedDiff(const QString& sText)
{
    QString sReturnValue(sText);
    QRegExp tRegExp(QString("(%[a-fA-F0-9]{2})"));

    while (tRegExp.indexIn(sReturnValue) != -1)
    {
        int iChar = tRegExp.cap(1).right(2).toInt();
        sReturnValue.replace(tRegExp.cap(1), QChar(iChar));
    }

    return sReturnValue;
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
