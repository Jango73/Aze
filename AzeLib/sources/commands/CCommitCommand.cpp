
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CCommitCommand.h"
#include "../CRepository.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CCommitCommand::CCommitCommand(CRepository* pRepository, const QString& sAuthor, const QString& sMessage)
    : CBaseCommand(pRepository)
    , m_sAuthor(sAuthor)
    , m_sMessage(sMessage)
{
}

//-------------------------------------------------------------------------------------------------

bool CCommitCommand::execute()
{
    OUT_DEBUG(QString("sAuthor=%1").arg(m_sAuthor));
    OUT_DEBUG(QString("sMessage=%1").arg(m_sMessage));

    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    OUT_DEBUG(QString("current branch=%1").arg(m_pRepository->currentBranchName()));

    // Check presence and relevancy of staging commmit
    if (IS_NULL(m_pRepository->stagingCommit()) || m_pRepository->stagingCommit()->files().count() == 0)
    {
        OUT_ERROR(CStrings::s_sTextCommitEmpty);
        return false;
    }

    // Create a new commit object
    CCommit* pNewCommit = nullptr;

    // Read the tip commit of the current branch
    if (not IS_NULL(m_pRepository->tipCommit()))
    {
        OUT_DEBUG(QString("Existing tip commit id=%1").arg(m_pRepository->tipCommit()->id()));

        pNewCommit = m_pRepository->tipCommit()->clone(this);
        pNewCommit->clearParents();
        pNewCommit->addParent(m_pRepository->tipCommit()->id());
    }
    else
    {
        OUT_DEBUG("No current tip commit");

        pNewCommit = new CCommit(this);
    }

    // Add the staged commmit to this new commit
    pNewCommit->addCommit(m_pRepository->database(), m_pRepository->stagingCommit());

    // Finalize and save the commit
    // We need all the info to generate the commit's id
    pNewCommit->setAuthor(m_sAuthor);
    pNewCommit->setMessage(m_sMessage);

    QString sNewCommitId = pNewCommit->generateId();
    pNewCommit->toFile(m_pRepository->database()->composeCommitFileName(sNewCommitId));

    // Update the current branch
    m_pRepository->currentBranch()->setTipCommitId(sNewCommitId);

    if (m_pRepository->currentBranch()->rootCommitId().isEmpty())
    {
        m_pRepository->currentBranch()->setRootCommitId(sNewCommitId);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
