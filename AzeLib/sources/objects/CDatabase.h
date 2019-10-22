
#pragma once

// Global
#include "../Aze_global.h"

// Qt
#include <QObject>

// qt-plus
#include "Macros.h"

// Application
#include "../CEnums.h"
#include "../CStrings.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CDatabase : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

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
    CDatabase(const QString& sRootPath, QObject* parent = nullptr);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    bool init();

    //! Returns a relative file name given a root path and an absolute path to a file
    QString relativeFileName(const QString& sFileName);

    //!
    QString absoluteFileName(const QString& sFileName);

    //!
    QString composeLocalFileName(const QString& sFileName);

    //!
    QString composeBranchFileName(const QString& sBranchName);

    //!
    QString composeCommitFileName(const QString& sCommitId);

    //!
    QString composeObjectFileName(const QString& sId);

    //!
    QString storeFile(const QString& sFileName);

    //!
    bool fileWithIdExists(const QString& sId);

    //!
    QByteArray fileContent(const QString& sId);

    //!
    QString printableFileContentById(const QString& sId);
};

}
