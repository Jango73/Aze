
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
#include "../CEnums.h"
#include "../CStrings.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class CDatabase;

class AZE_SHARED_EXPORT CCommit : public CObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // QML properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(bool, b, isMerge, IsMerge)
    Q_FAST_PROPERTY(QString, s, author, Author)
    Q_FAST_PROPERTY(QString, s, date, Date)
    Q_FAST_PROPERTY(QString, s, message, Message)
    Q_FAST_PROPERTY(QStringList, l, parents, Parents)
    Q_FAST_PROPERTY(QDictionary, m, user, User)
    Q_FAST_PROPERTY(QDictionary, m, files, Files)

    // This list will be filled only by calling readParents
    Q_FAST_PROPERTY(QList<CCommit*>, l, parentCommits, ParentCommits)

    // For internal use
    Q_FAST_PROPERTY(int, i, distance, Distance)
    Q_FAST_PROPERTY(int, i, tag, Tag)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CCommit(QObject* parent = nullptr);

    //!
    virtual ~CCommit() override;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CCommit* clone(QObject* parent = nullptr) const;

    //!
    CXMLNode toNode() const override;

    //!
    bool toFile(const QString& sFileName) const;

    //!
    void setDateToNow();

    //!
    void clearParents();

    //!
    void readParentCommits(CDatabase* pDatabase);

    //!
    void addParent(const QString& sParentId);

    //!
    bool addFile(CDatabase* pDatabase, QString sRelativeFileName, QString sId = "");

    //!
    bool removeFile(QString sRelativeFileName);

    //!
    bool addCommit(CDatabase* pDatabase, const CCommit* pCommitToAdd);

    //!
    QByteArray fileContent(CDatabase* pDatabase, QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Static control methods
    //-------------------------------------------------------------------------------------------------

public:

    //!
    static CCommit* fromNode(const CXMLNode& xNode, QObject* parent, QString sCommitId);

    //!
    static CCommit* fromFile(const QString& sFileName, QObject* parent, QString sCommitId);

    //!
    static CCommit* fromId(CDatabase* pDatabase, QString sCommitId, QObject* parent);

    //!
    static QList<CCommit*> parentList(CDatabase* pDatabase, const CCommit* pCommit, QObject* parent);

    //!
    static QStringList parentIds(CDatabase* pDatabase, const QString& sCommitId);
};

}
