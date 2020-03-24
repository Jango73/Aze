
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QDateTime>

// qt-plus
#include "Macros.h"

// Application
#include "CRemoteHostInfo.h"
#include "CCommitFunctions.h"
#include "CEnums.h"
#include "CStrings.h"
#include "objects/CBranch.h"
#include "objects/CCommit.h"
#include "objects/CDatabase.h"
#include "objects/CFile.h"
#include "objects/CObject.h"
#include "objects/CStash.h"

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
    Q_FAST_PROPERTY(CRemoteHostInfo*, p, remoteHostInfo, RemoteHostInfo)
    Q_FAST_PROPERTY(CCommitFunctions*, p, commitFunctions, CommitFunctions)
    Q_FAST_PROPERTY(CEnums::EFileStatus, e, status, Status)

    Q_FAST_PROPERTY(QString, s, currentBranchName, CurrentBranchName)
    Q_FAST_PROPERTY(QString, s, stagingCommitFileName, StagingCommitFileName)

    Q_FAST_PROPERTY(CBranch*, p, currentBranch, CurrentBranch)
    Q_FAST_PROPERTY(CCommit*, p, stagingCommit, StagingCommit)
    Q_FAST_PROPERTY(CCommit*, p, rootCommit, RootCommit)
    Q_FAST_PROPERTY(CCommit*, p, tipCommit, TipCommit)

    Q_FAST_PROPERTY(QStringList, l, stashList, StashList)

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

    //! Initializes a repository, creating mandatory folders and files
    bool init();

    //! Creates a branch as sName
    bool createBranch(const QString& sName);

    //! Sets the current branch to sName
    bool switchToBranch(const QString& sName, bool bAllowFileDelete = false);

    //! Stages all files listed in lFileNames
    bool stage(const QStringList& lFileNames);

    //! Unstages all files listed in lFileNames
    bool unstage(const QStringList& lFileNames);

    //! Removes all files listed in lFileNames
    bool remove(const QStringList& lFileNames);

    //! Commits all files contained in the staging commit
    bool commit(const QString& sAuthor, const QString& sMessage);

    //! Reverts all files listed in lFileNames
    bool revert(const QStringList& lFileNames, bool bAllowFileDelete = false);

    //! Reverts all files listed in the commit
    bool revert(CCommit* pWorkingDirectory, bool bAllowFileDelete = false);

    //! Reverts all files
    bool revert(bool bAllowFileDelete = false);

    //! Returns a log
    QString log(const QStringList& lFileNames, bool bGraph = false, int iStart = 0, int iCount = 0);

    //! Returns a list of files and their status
    QList<CFile> fileStatus(const QStringList& lFileNames);

    //! Makes a diff betweeen commits or files
    QString diff(const QString& sFirst, const QString& sSecond);

    //! Merges the branch sName onto the current one
    bool merge(const QString& sName);

    //! Stashes the working directory changes
    bool saveStash(const QString& sMessage);

    //! Restores the contents of a stash
    bool popStash(const QString& sId);

    //! Returns the staging commit
    CCommit* getStagingCommit();

    //! Reads the stage commit
    bool readGeneralInformation();

    //! Reads the current branch
    bool readCurrentBranch();

    //! Reads the general information
    bool readStage();

    //! Reads the current branch's root commit
    bool readRootCommit();

    //! Reads the current branch's tip commit
    bool readTipCommit();

    //! Writes the general information
    bool writeGeneralInformation();

    //! Writes the current branch
    bool writeCurrentBranch();

    //! Writes the stage commit
    bool writeStage();

    //! Clears the stage commit (but does not write it)
    bool clearStage();

    //! Checks the status of tracked files
    CEnums::EFileStatus checkStatus(const QStringList& lFileNames);

    //! Gets a list of loose files
    QStringList getLooseFiles();

    //! Adds a stash ID to the list of stash IDs
    void addStashToList(const QString& sId);

    //! Removes a stash ID from the list of stash IDs
    void removeStashFromList(const QString& sId);

    //!
    void setRemoteHostName(const QString& sName);

    //-------------------------------------------------------------------------------------------------
    // Helper methods
    //-------------------------------------------------------------------------------------------------

    //! Replaces symbols in sText with their actual value (if any)
    QString processKeywords(const QString& sText);

    //! Places in iDelta the value of delta symbols in sText (if any)
    QString processDeltas(const QString& sText, int& iDelta);

    //! Returns the diff between tip commit and working directory, if any
    QString diffWorkingDirectory();

    //!
    QString getPushRequest();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    QList<CFile> m_lFiles;
};

}
