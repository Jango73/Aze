
#pragma once

// Global
#include "Aze_global.h"

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

class AZE_SHARED_EXPORT CUtils
{
public:

    //-------------------------------------------------------------------------------------------------
    // Public static methods
    //-------------------------------------------------------------------------------------------------

    //!
    static QString relativeFileName(const QString& sRootPath, const QString& sFileName);

    //!
    static QString absoluteFileName(const QString& sRootPath, const QString& sFileName);

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
    static QString idFromString(const QString& sText);

    //!
    static QString idFromFile(const QString& sFilename);

    //!
    static QString packIdAndFile(const QString& sId, const QString& sFilePath);

    //!
    static void unpackIdAndFile(const QString& sPack, QString& sId, QString& sFilePath);
};

}
