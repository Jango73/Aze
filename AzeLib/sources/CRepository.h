
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QDateTime>

// qt-plus
#include "Macros.h"

// Application
#include "objects/CObject.h"
#include "objects/CBranch.h"
#include "objects/CCommit.h"
#include "objects/CDatabase.h"
#include "objects/CFile.h"
#include "CEnums.h"
#include "CStrings.h"

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
    Q_FAST_PROPERTY(CDatabase*, p, database, Database)

    Q_FAST_PROPERTY(QString, s, currentBranchName, CurrentBranchName)
    Q_FAST_PROPERTY(QString, s, stagingCommitFileName, StagingCommitFileName)

    Q_FAST_PROPERTY(CBranch*, p, currentBranch, CurrentBranch)
    Q_FAST_PROPERTY(CCommit*, p, stagingCommit, StagingCommit)
    Q_FAST_PROPERTY(CCommit*, p, rootCommit, RootCommit)
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
    // Command methods
    //-------------------------------------------------------------------------------------------------

    //!
    bool init();

    //! Creates a branch as sName
    bool createBranch(const QString& sName);

    //! Stages all files listed in lFileNames
    bool stage(const QStringList& lFileNames);

    //! Unstages all files listed in lFileNames
    bool unstage(const QStringList& lFileNames);

    //! Removes all files listed in lFileNames
    bool remove(const QStringList& lFileNames);

    //! Commits all files contained in the staging commit
    bool commit(const QString& sAuthor, const QString& sMessage);

    //! Makes a diff betweeen commits or files
    QString diff(const QString& sFirst, const QString& sSecond);

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

    //! Reads the current branch's root commit
    bool readRootCommit();

    //! Reads the current branch's tip commit
    bool readTipCommit();

    //! Writes the general info
    bool writeGeneralInfo();

    //! Writes the current branch
    bool writeCurrentBranch();

    //! Writes the stage commit
    bool writeStage();

    //! Clears the stage commit (but does not write it)
    bool clearStage();

    //-------------------------------------------------------------------------------------------------
    // Helper methods
    //-------------------------------------------------------------------------------------------------

    //!
    QString processKeywords(const QString& sText);

    //!
    QString processDeltas(const QString& sText, int& iDelta);

    //!
    CCommit* getCommitAncestor(CCommit* pCommit, int iDelta, QObject* parent);

    //!
    CCommit* workingDirectoryAsCommit();

    //!
    void listFilesRecursive(QStringList& lStack, QString sRootDirectory, QString sCurrentDirectory);
};

}
