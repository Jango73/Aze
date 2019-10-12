
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
#include "CBranch.h"
#include "CCommit.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CRepository : public CObject
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

    Q_FAST_PROPERTY(QString, s, currentBranchName, CurrentBranchName)
    Q_FAST_PROPERTY(CBranch*, p, currentBranch, CurrentBranch)

    Q_FAST_PROPERTY(QString, s, stagingCommitFileName, StagingCommitFileName)
    Q_FAST_PROPERTY(CCommit*, p, stagingCommit, StagingCommit)

    Q_FAST_PROPERTY(CCommit*, p, tipCommit, TipCommit)

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
    bool add(const QStringList& lFileNames);

    //!
    bool remove(const QStringList& lFileNames);

    //!
    bool commit(const QString& sAuthor, const QString& sMessage);

    //!
    CCommit* getStagingCommit();

    //!
    QList<CFile> fileStatus();

    //! Reads the stage commit
    bool readGeneralInfo();

    //! Reads the current branch
    bool readCurrentBranch();

    //! Reads general information
    bool readStage();

    //!
    bool readTipCommit();

    //!
    bool writeGeneralInfo();

    //!
    bool writeCurrentBranch();

    //! Writes the stage commit
    bool writeStage();

    //! Clears the stage commit (but does not write it)
    bool clearStage();

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
