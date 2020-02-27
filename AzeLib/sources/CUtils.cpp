
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

//-------------------------------------------------------------------------------------------------

void CUtils::ensureFilePathExists(const QString& sFilePath)
{
    if (sFilePath.contains(CStrings::s_sPathSep))
    {
        QString sFile = sFilePath.split(CStrings::s_sPathSep).last();
        QString sPath = sFilePath;
        sPath.replace(sFile, "");
        QDir().mkpath(sPath);
    }
}

//-------------------------------------------------------------------------------------------------

bool CUtils::moveFile(const QString& sSource, const QString& sTarget)
{
    QFile target(sTarget);
    target.remove();
    QFile source(sSource);
    return source.rename(sTarget);
}

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
    QCryptographicHash crypto(QCryptographicHash::Sha256);
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
    // SHA256 length = 32 bytes = 64 chars in hex

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    return (sId.count() / 2) == 32;
#else
    return (sId.count() / 2) == QCryptographicHash::hashLength(QCryptographicHash::Sha256);
#endif
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

    if (tInputFile.exists() && tInputFile.open(QIODevice::ReadOnly))
    {
        baData = tInputFile.readAll();
        tInputFile.close();
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
    ensureFilePathExists(sFileName);

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
    diff_match_patch diff;
    diff.Match_Threshold = 0.1f;
    diff.Patch_DeleteThreshold = 0.1f;

    return diff.patch_toText(diff.patch_make(sText1, sText2));
}

//-------------------------------------------------------------------------------------------------

QString CUtils::applyUnifiedDiff(const QString& sText, const QString& sDiff)
{
    diff_match_patch diff;
    diff.Match_Threshold = 0.1f;
    diff.Patch_DeleteThreshold = 0.1f;

    QList<Patch> patches = diff.patch_fromText(sDiff);
    QPair<QString, QVector<bool> > returnValue = diff.patch_apply(patches, sText);

    if (returnValue.second.contains(false))
        return "";

    return returnValue.first;
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

QList<QPair<QString, QString> > CUtils::splitDiff(const QString& sFullDiff)
{
    QList<QPair<QString, QString> > mReturnValue;

    QRegExp tRegExp1(QString("%1\\s+[a-zA-Z0-9\\.\\/\\+-_]+\\s+[a-zA-Z0-9\\.\\/\\+-_]+\\s*\\r*\\n")
                    .arg(CStrings::s_sDiffChunkHeader)
                    );
    QRegExp tRegExp2(QString("---\\s+([a-zA-Z0-9\\.\\/\\+-_]+)\\s*\\r*\\n"));
    QRegExp tRegExp3(QString("\\+\\+\\+\\s+([a-zA-Z0-9\\.\\/\\+-_]+)\\s*\\r*\\n"));

    // Get a list of file diffs from the full diff
    QStringList lFileDiffs = sFullDiff.split(tRegExp1, QString::SkipEmptyParts);

    // Iterate through each file diffs
    for (QString sFileDiff : lFileDiffs)
    {
        if (tRegExp2.indexIn(sFileDiff) != -1)
        {
            QString sFileName = tRegExp2.cap(1).trimmed();

            if (not sFileName.isEmpty())
            {
                // Remove things not expected by diff-match-patch
                sFileDiff.replace(tRegExp2, "");
                sFileDiff.replace(tRegExp3, "");

                mReturnValue << QPair<QString, QString>(sFileName, sFileDiff);
            }
        }
    }

    return mReturnValue;
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
