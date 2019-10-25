
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CDiffCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CDiffCommand::CDiffCommand(CRepository* pRepository, const QString& sFirst, const QString& sSecond, QString* pResult)
    : CBaseCommand(pRepository)
    , m_sFirst(sFirst)
    , m_sSecond(sSecond)
    , m_pResult(pResult)
{
}

//-------------------------------------------------------------------------------------------------

bool CDiffCommand::execute()
{
    QString sObject1 = m_sFirst;
    QString sObject2 = m_sSecond;
    int iDelta1 = 0;
    int iDelta2 = 0;
    bool bWorkOnCommits = false;

    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    if (sObject1.isEmpty() && sObject2.isEmpty())
    {
        sObject1 = m_pRepository->currentBranch()->rootCommitId();
    }

    sObject1 = m_pRepository->processDeltas(sObject1, iDelta1);
    sObject2 = m_pRepository->processDeltas(sObject2, iDelta2);

    sObject1 = m_pRepository->processKeywords(sObject1);
    sObject2 = m_pRepository->processKeywords(sObject2);

    QString sCommitFileName1 = m_pRepository->database()->composeCommitFileName(sObject1);
    QString sCommitFileName2 = m_pRepository->database()->composeCommitFileName(sObject2);

    OUT_DEBUG(QString("sCommitFileName1: %1").arg(sCommitFileName1));
    OUT_DEBUG(QString("sCommitFileName2: %1").arg(sCommitFileName2));

    // Work on commits if both object names are existing files
    if (QFile(sCommitFileName1).exists() && QFile(sCommitFileName2).exists())
    {
        bWorkOnCommits = true;
    }

    // Work on commits if object 1 is an existing file and object 2 is empty
    // We assume object 2 to be current stage
    if (QFile(sCommitFileName1).exists() && sObject2.isEmpty())
    {
        bWorkOnCommits = true;
    }

    // Work on commits if object 2 is an existing file and object 1 is empty
    // We assume object 1 to be current stage
    if (QFile(sCommitFileName2).exists() && sObject1.isEmpty())
    {
        bWorkOnCommits = true;
    }

    // The provided object(s) is a(are) commit(s)
    if (bWorkOnCommits)
    {
        CCommit* pCommit1 = nullptr;
        CCommit* pCommit2 = nullptr;

        if (not sObject1.isEmpty() && not sObject2.isEmpty())
        {
            pCommit1 = CCommit::fromFile(sCommitFileName1, this, sObject1);
            pCommit2 = CCommit::fromFile(sCommitFileName2, this, sObject2);
        }
        else if (not sObject1.isEmpty() && sObject2.isEmpty())
        {
            if (IS_NULL(m_pRepository->stagingCommit()))
                return false;

            pCommit1 = CCommit::fromFile(sCommitFileName1, this, sObject1);
            pCommit2 = m_pRepository->stagingCommit()->clone();
        }
        else if (sObject1.isEmpty() && not sObject2.isEmpty())
        {
            if (IS_NULL(m_pRepository->stagingCommit()))
                return false;

            pCommit1 = CCommit::fromFile(sCommitFileName2, this, sObject2);
            pCommit2 = m_pRepository->stagingCommit()->clone();
        }

        if (not IS_NULL(pCommit1) && not IS_NULL(pCommit2))
        {
            diffCommits(pCommit1, pCommit2, iDelta1, iDelta2);
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

void CDiffCommand::diffCommits(CCommit* pCommit1, CCommit* pCommit2, int iDelta1, int iDelta2)
{
    if (iDelta1 != 0)
        pCommit1 = m_pRepository->getCommitAncestor(pCommit1, this, iDelta1);

    if (iDelta2 != 0)
        pCommit2 = m_pRepository->getCommitAncestor(pCommit2, this, iDelta2);

    for (QString sName : pCommit2->files().values())
    {
        QByteArray baContent1 = pCommit1->fileContent(m_pRepository->database(), sName);
        QByteArray baContent2 = pCommit2->fileContent(m_pRepository->database(), sName);

        QString sDiffText = CUtils::unifiedDiff(QString(baContent1), QString(baContent2));

        if (not sDiffText.isEmpty())
        {
            (*m_pResult) += QString("diff --aze %1 %2%3%4")
                    .arg(sName)
                    .arg(sName)
                    .arg(CStrings::s_sNewLine)
                    .arg(sDiffText);
        }
    }
}

}
