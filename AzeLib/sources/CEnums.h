
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

namespace Aze {

class CEnums : public QObject
{
    Q_OBJECT

public:

    // Branch types
    enum EBranchType
    {
        eBranch,
        eTag
    };
};

typedef QMap<QString, QString> QDictionary;

}
