
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QList>

// qt-plus
#include "Macros.h"

// Application
#include "objects/CCommit.h"
#include "objects/CFile.h"
#include "objects/CDatabase.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CCommitFunctions : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(CDatabase*, p, database, Database)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CCommitFunctions(CDatabase* pDatabase, QObject* parent = nullptr);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CCommit* getCommitAncestor(CCommit* pCommit, QObject* owner = nullptr, int iDelta = 1);

    //!
    QList<CCommit*> getCommitAncestorList(
            CCommit* pCommit,
            QObject* owner = nullptr,
            bool bStayOnBranch = true,
            int iMaxCount = 0,
            QString sStopAtCommitId = ""
            );

    //!
    void getCommitAncestorListRecurse(
            QList<CCommit*>& lCommitList,
            CCommit* pCommit,
            QObject* owner = nullptr,
            int iDepth = 0,
            bool bStayOnBranch = true,
            int iGuard = 0,
            QString sStopAtCommitId = ""
            );

    //!
    CCommit* getCommonCommitChains(
            CCommit* pCommit1,
            CCommit* pCommit2,
            QObject* owner = nullptr,
            QList<CCommit*>* lCommit1Chain = nullptr,
            QList<CCommit*>* lCommit2Chain = nullptr
            );

    //!
    CCommit* directoryAsCommit(QObject* owner = nullptr, QString sRootPath = "");

    //!
    void listFilesRecursive(QStringList& lStack, QString sRootDirectory, QString sCurrentDirectory);

    //!
    void diffCommits(QString& sOutput, CCommit* pCommit1, CCommit* pCommit2, const QStringList& lIgnoreFiles, int iDelta1 = 0, int iDelta2 = 0);

    //!
    void diffCommitLists(QString& sOutput, const QList<CCommit*>& lCommitList, const QList<CCommit*>& lIgnoreList);

    //!
    void diffFiles(QString& sOutput, const QString& sFile1, const QString& sFile2);

    //!
    void diffText(QString& sOutput, const QString& sFileName, const QString& sText1, const QString& sText2);

    //! Applies a diff to the working directory
    bool applyDiff(const QString& sFullDiff, bool bAddToStage = false, CCommit* pStagingCommit = nullptr);
};

}
