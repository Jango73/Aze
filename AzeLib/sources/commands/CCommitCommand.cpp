
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
    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

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
        pNewCommit = m_pRepository->tipCommit()->clone(this);
        pNewCommit->clearParents();
        pNewCommit->addParent(m_pRepository->tipCommit()->id());
    }
    else
    {
        pNewCommit = new CCommit(this);
    }

    // Add the staged commmit to this new commit
    pNewCommit->addCommit(m_pRepository->database(), m_pRepository->stagingCommit());

    if (m_sMessage.isEmpty())
        m_sMessage = m_pRepository->stagingCommit()->message();

    // Finalize and save the commit
    // We need all the info to generate the commit's id
    pNewCommit->setIsMerge(m_pRepository->stagingCommit()->isMerge());
    pNewCommit->setDateToNow();
    pNewCommit->setAuthor(m_sAuthor);
    pNewCommit->setMessage(m_sMessage);

    QString sNewCommitId = m_pRepository->database()->storeCommit(pNewCommit);

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
