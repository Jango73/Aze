
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
#include "CObject.h"
#include "CCommit.h"
#include "../CEnums.h"
#include "../CStrings.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class CDatabase;

class AZE_SHARED_EXPORT CStash : public CObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // QML properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(QString, s, author, Author)
    Q_FAST_PROPERTY(QString, s, message, Message)
    Q_FAST_PROPERTY(QString, s, diff, Diff)
    Q_FAST_PROPERTY(CCommit*, p, stage, Stage)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CStash(QObject* parent = nullptr);

    //!
    virtual ~CStash() override;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CXMLNode toNode() const override;

    //!
    bool toFile(const QString& sFileName) const;

    //-------------------------------------------------------------------------------------------------
    // Static control methods
    //-------------------------------------------------------------------------------------------------

public:

    //!
    static CStash* fromNode(const CXMLNode& xNode, QObject* parent, QString sStashId);

    //!
    static CStash* fromFile(const QString& sFileName, QObject* parent, QString sStashId);
};

}
