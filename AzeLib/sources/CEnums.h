
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QMetaEnum>
#include <QString>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CEnums : public QObject
{
    Q_OBJECT

public:

    // Branch types
    enum EBranchType
    {
        eBranch,
        eTag
    };

    // File status
    enum EFileStatus
    {
        eLoose,
        eClean,
        eModified,
        eAdded,
        eDeleted,
        eIgnored
    };

    Q_ENUM(EBranchType)
    Q_ENUM(EFileStatus)

    //-------------------------------------------------------------------------------------------------
    // Translators
    //-------------------------------------------------------------------------------------------------

    static QString FileStatusSymbol(EFileStatus eStatus);
};

//-------------------------------------------------------------------------------------------------
// Some typedefs

typedef QMap<QString, QString> QDictionary;

}
