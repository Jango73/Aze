
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QDateTime>

// qt-plus
#include "Macros.h"

// Application
#include "CObject.h"
#include "CEnums.h"
#include "CStrings.h"
#include "CFile.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZESHARED_EXPORT CRepository : public CObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(bool, b, ok, Ok)
    Q_FAST_PROPERTY(QString, s, rootPath, RootPath)
    Q_FAST_PROPERTY(QString, s, dataPath, DataPath)
    Q_FAST_PROPERTY(QString, s, stashPath, StashPath)
    Q_FAST_PROPERTY(QString, s, branchPath, BranchPath)
    Q_FAST_PROPERTY(QString, s, commitPath, CommitPath)
    Q_FAST_PROPERTY(QString, s, objectPath, ObjectPath)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CRepository(const QString& sRootPath, QObject* parent = nullptr);

    //!
    ~CRepository();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    bool init();

    //!
    bool add(const QStringList& lRelativeFileName);

    //!
    bool remove(const QStringList& lRelativeFileName);

    //!
    QList<CFile> fileStatus();

    //!
    QString absoluteFileName(const QString& sFileName);

    //!
    QString relativeFileName(const QString& sFileName);

    //-------------------------------------------------------------------------------------------------
    // Protected control methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    bool addSingleFile(QString sRelativeFileName);

    //!
    bool removeSingleFile(QString sRelativeFileName);
};

}
