
#pragma once

// Qt
#include <QObject>
#include <QDateTime>

// qt-plus
#include "Macros.h"
#include "CXMLNode.h"

// Application
#include "CEnums.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class CUtils
{
public:

    //-------------------------------------------------------------------------------------------------
    // Public static methods
    //-------------------------------------------------------------------------------------------------

    //!
    static QDictionary dictionaryFromNode(const CXMLNode& xNode);

    //!
    static CXMLNode nodeFromDictionary(const QDictionary& xDict);

    //!
    static QString serializeBase64(const QString& sText);

    //!
    static QString deserializeBase64(const QString& sText);

    //!
    static QString idFromByteArray(const QByteArray& baData);

    //!
    static QString idFromFile(const QString& sFilename);
};

}
