
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

#define PRINTABLE_ISO_DATE(a)   (QDateTime::fromString(a, Qt::ISODate).toUTC().toString("yyyy-MM-dd hh:mm"))

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CUtils
{
public:

    //-------------------------------------------------------------------------------------------------
    // Public static methods
    //-------------------------------------------------------------------------------------------------

    //!
    static void ensureFilePathExists(const QString& sFileName);

    //!
    static bool moveFile(const QString& sSource, const QString& sTarget);

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
    static bool putBinaryFileContent(const QString& sFileName, const QByteArray& baContent);

    //!
    static bool putTextFileContent(const QString& sFileName, const QString& sContent);

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
    static QString idFromFileContent(const QString& sFilename);

    //!
    static bool idValid(const QString& sId);

    //!
    static QString packIdAndFile(const QString& sId, const QString& sFilePath);

    //!
    static void unpackIdAndFile(const QString& sPack, QString& sId, QString& sFilePath);

    //!
    static std::vector<std::string> textToStdStringVector(const QString& sText);

    //!
    static QString fileDiffHeader(const QString& sLeft, const QString& sRight);

    //!
    static QString unifiedDiff(const QString& sText1, const QString& sText2);

    //!
    static QString applyUnifiedDiff(const QString& sText, const QString& sDiff);

    //!
    static QString printableUnifiedDiff(const QString& sText);
};

}
