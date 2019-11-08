
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

QList<CCommit*> CCommitFunctions::getCommitAncestorList(CCommit* pCommit, QObject* owner, bool bStayOnBranch, int iMaxCount)
{
    int iGuard = iMaxCount == 0 ? 999999 : iMaxCount;
    QList<CCommit*> lReturnValue;
    getCommitAncestorListRecurse(lReturnValue, pCommit, owner, bStayOnBranch, iGuard);
    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::getCommitAncestorListRecurse(QList<CCommit*>& lCommitList, CCommit* pCommit, QObject* owner, bool bStayOnBranch, int iGuard)
{
    if (iGuard <= 0)
        return;

    iGuard--;

    QList<CCommit*> parents = CCommit::parentList(m_pDatabase, pCommit, owner);

    if (bStayOnBranch)
    {
        if (parents.count() > 0)
        {
            lCommitList << parents[0];
            getCommitAncestorListRecurse(lCommitList, parents[0], owner, bStayOnBranch, iGuard - 1);
        }
    }
    else
    {
        for (CCommit* pParent : parents)
        {
            lCommitList << pParent;
            getCommitAncestorListRecurse(lCommitList, pParent, owner, bStayOnBranch, iGuard - 1);
        }
    }
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommitFunctions::getCommonCommitChains(CCommit* pCommit1, CCommit* pCommit2, QObject* owner, QList<CCommit*>* lCommit1Chain, QList<CCommit*>* lCommit2Chain)
{
    CCommit* pAncestor = nullptr;
    QList<CCommit*> pAncestors1 = getCommitAncestorList(pCommit1, owner, false);
    QList<CCommit*> pAncestors2 = getCommitAncestorList(pCommit2, owner, true);

    // Add commits themselves to lists
    pAncestors1.prepend(pCommit1->clone());
    pAncestors2.prepend(pCommit2->clone());

//    OUT_DEBUG("--------------------");
//    OUT_DEBUG("pAncestors1:");
//    for (CCommit* pCommit : pAncestors1OnBranch)
//        OUT_DEBUG(QString("%1 %2").arg(pCommit->id()).arg(pCommit->message()));
//    OUT_DEBUG("--------------------");
//    OUT_DEBUG("pAncestors2:");
//    for (CCommit* pCommit : pAncestors2)
//        OUT_DEBUG(QString("%1 %2").arg(pCommit->id()).arg(pCommit->message()));
//    OUT_DEBUG("--------------------");

    bool bFound = false;

    for (CCommit* pCommit1 : pAncestors1)
    {
        QString sId = pCommit1->id();

        for (CCommit* pCommit2 : pAncestors2)
        {
            if (sId == pCommit2->id())
            {
                pAncestor = pCommit1->clone(owner);
                OUT_INFO(QString(CStrings::s_sTextCommonAncestor).arg(pAncestor->id()));
                bFound = true;
                break;
            }
        }

        if (bFound)
            break;
    }

    // Populate the commit lists if required
    if (IS_NOT_NULL(lCommit1Chain))
    {
        for (CCommit* pCommit : pAncestors1)
        {
            (*lCommit1Chain) << pCommit;

            if (pCommit == pAncestor)
                break;
        }

        REVERSE_LIST(*lCommit1Chain);
    }

    if (IS_NOT_NULL(lCommit2Chain))
    {
        for (CCommit* pCommit : pAncestors2)
        {
            (*lCommit2Chain) << pCommit;

            if (pCommit == pAncestor)
                break;
        }

        REVERSE_LIST(*lCommit2Chain);
    }

    // Delete the ancestor lists if required
    if (IS_NULL(lCommit1Chain))
        qDeleteAll(pAncestors1);

    if (IS_NULL(lCommit2Chain))
        qDeleteAll(pAncestors2);

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
        pNewCommit->addFile(sFile);

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
        QString sTargetDirectory = QString("%1/%2").arg(sCurrentDirectory).arg(iFile.fileName());
        listFilesRecursive(lStack, sRootDirectory, sTargetDirectory);
    }
}

//-------------------------------------------------------------------------------------------------

void CCommitFunctions::diffCommits(QString& sOutput, CCommit* pCommit1, CCommit* pCommit2, const QStringList& lIgnoreFiles, int iDelta1, int iDelta2)
{
    if (iDelta1 != 0)
        pCommit1 = getCommitAncestor(pCommit1, this, iDelta1);

    if (iDelta2 != 0)
        pCommit2 = getCommitAncestor(pCommit2, this, iDelta2);

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
//            OUT_DEBUG(QString("Diffing %1 & %2").arg(pCommit1->message()).arg(pCommit2->message()));

            for (QString sName : pCommit2->files().values())
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

void CCommitFunctions::diffFiles(QString& sOutput, const QString& sFile1, const QString& sFile2)
{
    QString sText1 = CUtils::getTextFileContent(sFile1);
    QString sText2 = CUtils::getTextFileContent(sFile2);

    QString sDiffText = CUtils::unifiedDiff(sText1, sText2);

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

}
