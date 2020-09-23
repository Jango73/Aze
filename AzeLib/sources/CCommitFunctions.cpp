
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CCommitFunctions.h"
#include "CEnums.h"
#include "CStrings.h"
#include "CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CCommitFunctions::CCommitFunctions(CDatabase* pDatabase, QObject* parent)
    : QObject(parent)
    , m_pDatabase(pDatabase)
{
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommitFunctions::getCommitAncestor(CCommit* pCommit, QObject* owner, int iDelta)
{
    if (IS_NULL(pCommit))
        return nullptr;

    CCommit* pAncestor = pCommit;
    int iGuard = 999999;

    while (true)
    {
        QList<CCommit*> parents = CCommit::parentList(m_pDatabase, pAncestor, owner);

        if (parents.count() == 0)
        {
            qDeleteAll(parents);
            pAncestor = nullptr;
            break;
        }

        // Delete unused data
        if (pAncestor != pCommit)
            delete pAncestor;

        // The first parent is the one to follow in order to stay on branch of pCommit
        pAncestor = parents[0]->clone(owner);

        // Delete unused data
        qDeleteAll(parents);

        iDelta--;
        iGuard--;

        if (iDelta == 0 || iGuard == 0)
            return pAncestor;
    }

    return pAncestor;
}

//-------------------------------------------------------------------------------------------------

QList<CCommit*> CCommitFunctions::getCommitAncestorList(CCommit* pCommit, QObject* owner, bool bStayOnBranch, int iMaxCount, QString sStopAtCommitId)
{
    int iGuard = iMaxCount == 0 ? 999999 : iMaxCount;
    int iDepth = 0;
    QList<CCommit*> lReturnValue;

    getCommitAncestorListRecurse(lReturnValue, pCommit, owner, iDepth, bStayOnBranch, iGuard, sStopAtCommitId);

    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::getCommitAncestorListRecurse(QList<CCommit*>& lCommitList, CCommit* pCommit, QObject* owner, int iDepth, bool bStayOnBranch, int iGuard, QString sStopAtCommitId)
{
    if (iGuard <= 0)
        return;

    iGuard--;

    QList<CCommit*> lParents = CCommit::parentList(m_pDatabase, pCommit, owner);

    if (bStayOnBranch)
    {
        if (lParents.count() > 0)
        {
            if (lParents[0]->id() != sStopAtCommitId)
            {
                lParents[0]->setDistance(iDepth);
                lCommitList << lParents[0];
                getCommitAncestorListRecurse(lCommitList, lParents[0], owner, iDepth + 1, bStayOnBranch, iGuard - 1, sStopAtCommitId);
            }
        }
    }
    else
    {
        for (CCommit* pParent : lParents)
        {
            if (pParent->id() != sStopAtCommitId)
            {
                pParent->setDistance(iDepth);
                lCommitList << pParent;
                getCommitAncestorListRecurse(lCommitList, pParent, owner, iDepth + 1, bStayOnBranch, iGuard - 1, sStopAtCommitId);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommitFunctions::getCommonCommitChains(CCommit* pCommit1, CCommit* pCommit2, QObject* owner, QList<CCommit*>* lCommit1Chain, QList<CCommit*>* lCommit2Chain)
{
    CCommit* pAncestor = nullptr;
    QList<CCommit*> pAncestors1 = getCommitAncestorList(pCommit1, owner, false);
    QList<CCommit*> pAncestors2 = getCommitAncestorList(pCommit2, owner, false);
    QMap<int, CCommit*> commonAncestors;

    // Add commits themselves to lists
    pAncestors1.prepend(pCommit1->clone(this));
    pAncestors2.prepend(pCommit2->clone(this));

//    OUT_DEBUG("--------------------");
//    OUT_DEBUG("pAncestors1:");
//    for (CCommit* pCommit : pAncestors1)
//        OUT_DEBUG(QString("%1 %2").arg(pCommit->id()).arg(pCommit->message()));
//    OUT_DEBUG("--------------------");
//    OUT_DEBUG("pAncestors2:");
//    for (CCommit* pCommit : pAncestors2)
//        OUT_DEBUG(QString("%1 %2").arg(pCommit->id()).arg(pCommit->message()));
//    OUT_DEBUG("--------------------");

    for (CCommit* pCommit1 : pAncestors1)
    {
        QString sId = pCommit1->id();

        for (CCommit* pCommit2 : pAncestors2)
        {
            if (sId == pCommit2->id())
            {
                int iDistance = pCommit1->distance() + pCommit2->distance();

                if (not commonAncestors.contains(iDistance))
                    commonAncestors[iDistance] = pCommit1;
            }
        }
    }

    // Bail out if no common ancestor found
    if (commonAncestors.count() == 0)
        return nullptr;

    // Clone the common ancestor
    int iKey = commonAncestors.keys()[0];
    pAncestor = commonAncestors[iKey]->clone(owner);

//    OUT_DEBUG(QString(CStrings::s_sTextCommonAncestor).arg(iKey).arg(pAncestor->message()));

    // Populate the commit lists if required
    if (IS_NOT_NULL(lCommit1Chain))
    {
        for (CCommit* pCommit : pAncestors1)
        {
            (*lCommit1Chain) << pCommit;

            if (pCommit == pAncestor)
                break;
        }

        reverseList(*lCommit1Chain);
    }

    if (IS_NOT_NULL(lCommit2Chain))
    {
        for (CCommit* pCommit : pAncestors2)
        {
            (*lCommit2Chain) << pCommit;

            if (pCommit == pAncestor)
                break;
        }

        reverseList(*lCommit2Chain);
    }

    // Delete the ancestor lists if required
    if (IS_NULL(lCommit1Chain))
        qDeleteAll(pAncestors1);

    if (IS_NULL(lCommit2Chain))
        qDeleteAll(pAncestors2);

//    if (IS_NOT_NULL(lCommit1Chain) && IS_NOT_NULL(lCommit2Chain))
//    {
//        OUT_DEBUG("--------------------");
//        OUT_DEBUG("lCommit1Chain:");
//        for (CCommit* pCommit : *lCommit1Chain)
//            OUT_DEBUG(QString("%1 %2").arg(pCommit->shortId()).arg(pCommit->message()));
//        OUT_DEBUG("--------------------");
//        OUT_DEBUG("lCommit2Chain:");
//        for (CCommit* pCommit : *lCommit2Chain)
//            OUT_DEBUG(QString("%1 %2").arg(pCommit->shortId()).arg(pCommit->message()));
//        OUT_DEBUG("--------------------");
//    }

    return pAncestor;
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommitFunctions::directoryAsCommit(QObject* owner, QString sRootPath)
{
    CCommit* pNewCommit = new CCommit(owner);
    QStringList lFiles;

    if (sRootPath.isEmpty())
        sRootPath = m_pDatabase->rootPath();

    listFilesRecursive(lFiles, sRootPath, sRootPath);

    // Add agregated files to the commit
    // The id will be generated from the file's contents
    for (QString sFile : lFiles)
        pNewCommit->addFile(m_pDatabase, sFile);

    return pNewCommit;
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::listFilesRecursive(QStringList& lStack, QString sRootDirectory, QString sCurrentDirectory)
{
    QStringList lFilter; lFilter << "*";

    QDir dDirectory(sCurrentDirectory);
    QFileInfoList lFiles = dDirectory.entryInfoList(lFilter, QDir::Files | QDir::NoSymLinks);

    for (QFileInfo iFile : lFiles)
    {
        QString sFullName = QString("%1/%2").arg(iFile.absolutePath()).arg(iFile.fileName());
        lStack << CUtils::relativeFileName(sRootDirectory, sFullName);
    }

    lFiles = dDirectory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    for (QFileInfo iFile : lFiles)
    {
        QString sDirectoryName = iFile.fileName();

        // Proceed recursively if we are not entering the .aze directory
        if (sDirectoryName != CStrings::s_sPathAzeDataRootPath)
        {
            QString sTargetDirectory = QString("%1/%2").arg(sCurrentDirectory).arg(sDirectoryName);
            listFilesRecursive(lStack, sRootDirectory, sTargetDirectory);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::diffCommits(QString& sOutput, CCommit* pCommit1, CCommit* pCommit2, const QStringList& lIgnoreFiles, int iDelta1, int iDelta2)
{
    if (iDelta1 != 0)
        pCommit1 = getCommitAncestor(pCommit1, this, iDelta1);

    if (iDelta2 != 0)
        pCommit2 = getCommitAncestor(pCommit2, this, iDelta2);

    if (IS_NOT_NULL(pCommit1) && IS_NOT_NULL(pCommit2))
    {

        for (QString sName : pCommit2->files().values())
        {
            if (not lIgnoreFiles.contains(sName))
            {
                QByteArray baContent1 = pCommit1->fileContent(m_pDatabase, sName);
                QByteArray baContent2 = pCommit2->fileContent(m_pDatabase, sName);

                QString sDiffText = CUtils::unifiedDiff(QString(baContent1), QString(baContent2));

                if (not sDiffText.isEmpty())
                {
                    sOutput += CUtils::fileDiffHeader(sName, sName);
                    sOutput += sDiffText;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::diffCommitLists(QString& sOutput, const QList<CCommit*>& lToList, const QList<CCommit*>& lFromList)
{
//    OUT_DEBUG("--------------------");
//    OUT_DEBUG("To list:");
//    for (CCommit* pCommit : lToList)
//        OUT_DEBUG(QString("%1 %2").arg(pCommit->id()).arg(pCommit->message()));

//    OUT_DEBUG("--------------------");
//    OUT_DEBUG("From list:");
//    for (CCommit* pCommit : lFromList)
//        OUT_DEBUG(QString("%1 %2").arg(pCommit->id()).arg(pCommit->message()));

//    OUT_DEBUG("--------------------");

    for (int index = 1; index < lFromList.count(); index++)
    {
        CCommit* pCommit1 = lFromList[index - 1];
        CCommit* pCommit2 = lFromList[index];

//        QList<CCommit*>::const_iterator foundCommit = std::find_if(
//                    lIgnoreList.begin(),
//                    lIgnoreList.end(),
//                    [&](const CCommit*& c) { return c->id() == pCommit2->id(); }
//        );

        bool bFound = false;

        for (CCommit* pCommit : lToList)
            if (pCommit->id() == pCommit2->id())
                { bFound = true; break; }

//        if (foundCommit == lIgnoreList.end())
        if (bFound == false)
        {
            for (QString sName : pCommit2->files().values())
            {
                QByteArray baContent1 = pCommit1->fileContent(m_pDatabase, sName);
                QByteArray baContent2 = pCommit2->fileContent(m_pDatabase, sName);

//                OUT_DEBUG(QString("X: %1").arg(QString(baContent1)));
//                OUT_DEBUG(QString("Y: %1").arg(QString(baContent2)));

                QString sDiffText = CUtils::unifiedDiff(QString(baContent1), QString(baContent2));

//                OUT_DEBUG(QString("Z: %1").arg(CUtils::printableUnifiedDiff(sDiffText)));

                if (not sDiffText.isEmpty())
                {
                    sOutput += CUtils::fileDiffHeader(sName, sName);
                    sOutput += sDiffText;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::diffFiles(QString& sOutput, const QString& sFile1, const QString& sFile2)
{
    // Get text content of both files
    QString sText1 = CUtils::getTextFileContent(sFile1);
    QString sText2 = CUtils::getTextFileContent(sFile2);

    // Diff text contents
    QString sDiffText = CUtils::unifiedDiff(sText1, sText2);

    // Output diff
    if (not sDiffText.isEmpty())
    {
        sOutput += CUtils::fileDiffHeader(sFile1, sFile2);
        sOutput += sDiffText;
    }
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::diffText(QString& sOutput, const QString& sFileName, const QString& sText1, const QString& sText2)
{
    QString sDiffText = CUtils::unifiedDiff(sText1, sText2);

    if (not sDiffText.isEmpty())
    {
        sOutput += CUtils::fileDiffHeader(sFileName, sFileName);
        sOutput += sDiffText;
    }
}

//-------------------------------------------------------------------------------------------------

bool CCommitFunctions::applyDiff(const QString& sFullDiff, bool bSilent, bool bAddToStage, CCommit* pStagingCommit)
{
    // Keep track of merged files
    QDictionary mProcessedFiles;
    QStringList mMovedFiles;

    // Get a list of diffs per file
    QList<QPair<QString, QString> > mFileDiffs = CUtils::splitDiff(sFullDiff);

    for (QPair<QString, QString> tPair : mFileDiffs)
    {
        QString sFileName = tPair.first;
        QString sFileDiff = tPair.second;

        // Read the file, apply the diff and write to merge directory
        QString sFullSourceName = m_pDatabase->composeLocalFileName(sFileName);
        QString sFullTargetName = m_pDatabase->composeMergeFileName(sFileName);

        // Check if file is already present in merge directory
        bool bMergedFileExists = QFile(sFullTargetName).exists();

        if (bMergedFileExists)
            sFullSourceName = sFullTargetName;

        // Get the content of the file
        QString sPreviousContent = CUtils::getTextFileContent(sFullSourceName);
        QString sNewContent = CUtils::applyUnifiedDiff(sPreviousContent, sFileDiff);

//        OUT_DEBUG(QString("A: %1").arg(sPreviousContent));
//        OUT_DEBUG(QString("B: %1").arg(CUtils::printableUnifiedDiff(sFileDiff)));
//        OUT_DEBUG(QString("C: %1").arg(sNewContent));
//        OUT_DEBUG("----------------------------------------");

        if (sPreviousContent.isEmpty() && sNewContent.isEmpty())
        {
            if (!bSilent)
            {
                OUT_ERROR(QString(CStrings::s_sTextCouldNotApplyPatch).arg(sFileName));
            }
            return false;
        }

        CUtils::putTextFileContent(sFullTargetName, sNewContent);
        QString sId = CUtils::idFromByteArray(sNewContent.toUtf8());
        mProcessedFiles[sId] = sFileName;
    }

    // Stage the merged files
    if (bAddToStage && IS_NOT_NULL(pStagingCommit))
    {
        for (QString sFileName : mProcessedFiles.values())
        {
            QString sKey = mapKeyForValue(mProcessedFiles, sFileName);
            pStagingCommit->addFile(m_pDatabase, mProcessedFiles[sKey], sFileName);
        }
    }

    // Move the merged files
    for (QString sFileName : mProcessedFiles.values())
    {
        QString sFullSourceName = m_pDatabase->composeMergeFileName(sFileName);
        QString sFullTargetName = m_pDatabase->composeLocalFileName(sFileName);

        if (not mMovedFiles.contains(sFullSourceName))
        {
            if (not CUtils::moveFile(sFullSourceName, sFullTargetName))
            {
                if (!bSilent)
                {
                    OUT_ERROR(QString("Could not move file %1.").arg(sFullSourceName));
                }
                return false;
            }

            mMovedFiles << sFullSourceName;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
