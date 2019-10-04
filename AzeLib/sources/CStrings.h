
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

namespace Aze {

class CStrings : public QObject
{
    Q_OBJECT

public:

    static const QString s_sPathSep;
    static const QString s_sNewLine;
    static const QString s_sNewLineEscaped;
    static const QString s_sBase64Marker;
    static const QString s_sPathIdSeparator;

    static const QString s_sParamAuthor;
    static const QString s_sParamDate;
    static const QString s_sParamFiles;
    static const QString s_sParamInfo;
    static const QString s_sParamMessage;
    static const QString s_sParamUser;
};

}
