
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

class AZESHARED_EXPORT CEnums : public QObject
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
};

//-------------------------------------------------------------------------------------------------
// Some typedefs

typedef QMap<QString, QString> QDictionary;

//-------------------------------------------------------------------------------------------------
// From http://blubbqt.blogspot.com

// c = class name; e = enum name; v = enum value
#define enumToQString(c, e, v) \
    (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(#e)).valueToKey(v))

// c = class name; e = enum name; s = string value
#define qStringToEnum(c, e, s) \
    c::e(c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(#e)).keyToValue(s.toLatin1()))

// c = class name; f = flag name, v = flag value
#define flagToQString(c, f, v) \
    (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(#f)).valueToKeys(v))

// c = class name; f = flag name; s = string value
#define qStringToFlag(c, f, s) \
    c::f(c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(#f)).keysToValue(s.toLatin1()))

}
