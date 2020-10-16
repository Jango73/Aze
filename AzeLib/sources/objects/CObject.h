
#pragma once

// Global
#include "../Aze_global.h"

// Qt
#include <QObject>
#include <QDateTime>

// qt-plus
#include "Macros.h"
#include "CXMLNode.h"

// Application
#include "../CEnums.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CObject : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // QML properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY_NO_SET_IMPL(QString, s, id, Id)
    Q_PROPERTY(QString shortId READ shortId NOTIFY shortIdChanged)
    Q_FAST_PROPERTY(QString, s, date, Date)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CObject(QObject* parent = nullptr);

    //!
    virtual ~CObject() override;

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString shortId() const;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    QString generateId() const;

    //!
    void setDateToNow();

    //!
    virtual CXMLNode toNode() const;

signals:
    void shortIdChanged();
};

}
