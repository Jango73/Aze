
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CStrings : public QObject
{
    Q_OBJECT

public:

    static const QString s_sPathSep;
    static const QString s_sNewLine;
    static const QString s_sNewLineEscaped;
    static const QString s_sBase64Marker;
    static const QString s_sPathIdSeparator;
    static const QString s_sCompressedXMLExtension;

    static const QString s_sParamAuthor;
    static const QString s_sParamDate;
    static const QString s_sParamFiles;
    static const QString s_sParamInfo;
    static const QString s_sParamMessage;
    static const QString s_sParamRoot;
    static const QString s_sParamTip;
    static const QString s_sParamType;
    static const QString s_sParamUser;

    static const QString s_sPathAzeDataRoot;
    static const QString s_sPathAzeStashPath;
    static const QString s_sPathAzeBranchPath;
    static const QString s_sPathAzeCommitPath;
    static const QString s_sPathAzeObjectPath;

    static const QString s_sStagingCommitFileName;
};

}
