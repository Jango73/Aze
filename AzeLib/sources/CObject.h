
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

class AZE_SHARED_EXPORT CObject : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // QML properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(QString, s, id, Id)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CObject(QObject* parent = nullptr);

    //!
    ~CObject();
};

}
