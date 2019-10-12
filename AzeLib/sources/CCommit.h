
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
#include "CObject.h"
#include "CEnums.h"
#include "CStrings.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CCommit : public CObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // QML properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(QString, s, author, Author)
    Q_FAST_PROPERTY(QString, s, date, Date)
    Q_FAST_PROPERTY(QString, s, message, Message)
    Q_FAST_PROPERTY(QDictionary, m, user, User)
    Q_FAST_PROPERTY(QDictionary, m, files, Files)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CCommit(QObject* parent = nullptr);

    //!
    ~CCommit();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CCommit* clone() const;

    //!
    CXMLNode toNode() const;

    //!
    bool toFile(const QString& sFileName) const;

    //!
    bool addFile(QString sRelativeFileName, QString sId = "");

    //!
    bool removeFile(QString sRelativeFileName);

    //!
    bool add(const QString& sRootPath, const QString& sObjectPath, CCommit* pCommitToAdd);

    //-------------------------------------------------------------------------------------------------
    // Static control methods
    //-------------------------------------------------------------------------------------------------

public:

    //!
    static CCommit* fromNode(const CXMLNode& xNode);

    //!
    static CCommit* fromFile(const QString& sFileName);
};

}
