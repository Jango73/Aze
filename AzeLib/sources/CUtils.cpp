
// Qt
#include <QBuffer>
#include <QDataStream>
#include <QCryptographicHash>

// Application
#include "CUtils.h"
#include "CStrings.h"

namespace Aze {

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
    return QString(CStrings::s_sBase64Marker) + QString(sText.toLatin1().toBase64());
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
    return QString(crypto.result());
}

//-------------------------------------------------------------------------------------------------

QString CUtils::idFromFile(const QString& sFilename)
{
    Q_UNUSED(sFilename);
    return "";
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
