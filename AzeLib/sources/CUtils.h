
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

    //! Returns a relative file name given a root path and an absolute path to a file
    static QString relativeFileName(const QString& sRootPath, const QString& sFileName);

    //!
    static QString absoluteFileName(const QString& sRootPath, const QString& sFileName);

    //!
    static bool fileExists(const QString& sRootPath, const QString& sRelativeFileName);

    //!
    static QByteArray getBinaryFileContent(const QString& sFileName);

    //!
    static QString getTextFileContent(const QString& sFileName);

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
    static bool idValid(const QString& sId);

    //!
    static QString packIdAndFile(const QString& sId, const QString& sFilePath);

    //!
    static void unpackIdAndFile(const QString& sPack, QString& sId, QString& sFilePath);

    //!
    static std::vector<std::string> textToStdStringVector(const QString& sText);

    //!
    static QString unifiedDiff(const QString& sText1, const QString& sText2);
};

}
