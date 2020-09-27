
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CMergeCommand.h"
#include "../CRepository.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CMergeCommand::CMergeCommand(CRepository* pRepository, const QString& sFromBranch)
    : CBaseCommand(pRepository)
    , m_sFromBranch(sFromBranch)
{
}

//-------------------------------------------------------------------------------------------------

bool CMergeCommand::execute()
{
    // Check validity of arguments
    if (m_sFromBranch.isEmpty())
    {
        m_pRepository->tellError(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        m_pRepository->tellError(CStrings::s_sTextBranchNameEmpty);
        return false;
    }

    // Check presence of stage
    if (IS_NULL(m_pRepository->stagingCommit()))
    {
        m_pRepository->tellError(CStrings::s_sTextNoStagingCommit);
        return false;
    }

    // Check presence of current branch tip commit
    if (IS_NULL(m_pRepository->tipCommit()))
    {
        m_pRepository->tellError(CStrings::s_sTextNoTipCommit);
        return false;
    }

    CCommit* pToTipCommit = m_pRepository->tipCommit();

    CBranch* pFromBranch = m_pRepository->database()->getBranch(m_sFromBranch, this);

    // Check presence of 'from' branch tip commit
    if (IS_NULL(pFromBranch) || pFromBranch->tipCommitId().isEmpty())
    {
        m_pRepository->tellError(CStrings::s_sTextNoSuchBranch);
        return false;
    }

    // Check if trying to merge a commit on itself (yes it may happen...)
    if (pFromBranch->tipCommitId() == m_pRepository->tipCommit()->id())
    {
        m_pRepository->tellError(CStrings::s_sTextCannotMergeSameCommits);
        return false;
    }

    CCommit* pFromTipCommit = m_pRepository->database()->getCommit(pFromBranch->tipCommitId(), this);

    // Check validity of 'from' branch tip commit
    if (IS_NULL(pFromTipCommit))
    {
        m_pRepository->tellError(CStrings::s_sTextNoSuchBranch);
        return false;
    }

    // Get the common commit chains of the two commits
    // Lists are from root to tip
    QString sCommonAncestorId = m_pRepository->commitFunctions()->getCommonCommitChains(
                pToTipCommit->id(),
                pFromTipCommit->id(),
                nullptr,
                nullptr
                );

    // Check presence of common ancestor
    if (sCommonAncestorId.isEmpty())
    {
        m_pRepository->tellError(CStrings::s_sTextNoCommonAncestor);
        return false;
    }

    // Check relevancy of 'from' commit
    if (sCommonAncestorId == pFromTipCommit->id())
    {
        m_pRepository->tellError(CStrings::s_sTextNoCommitToMerge);
        return false;
    }

    // Check status of working directory
    m_pRepository->checkStatus(QStringList());

    if (m_pRepository->status() != CEnums::eClean)
    {
        m_pRepository->tellError(CStrings::s_sTextMergeWorkingDirectoryNotClean);
        return false;
    }

    CCommit* pCommonAncestor = CCommit::fromId(m_pRepository->database(), sCommonAncestorId, this);
    if (IS_NULL(pCommonAncestor))
    {
        m_pRepository->tellError(CStrings::s_sTextNoCommonAncestor);
        return false;
    }

    bool bMergeOK = m_pRepository->commitFunctions()->threeWayMerge(pCommonAncestor, pFromTipCommit, pToTipCommit, true, m_pRepository->stagingCommit());

    // Set merge information in staging commit
    // Only the 'from' commit is added as a parent here, the other will be added by commit command
    m_pRepository->stagingCommit()->setIsMerge(true);
    m_pRepository->stagingCommit()->clearParents();
    m_pRepository->stagingCommit()->addParent(pFromTipCommit->id());
    m_pRepository->stagingCommit()->setMessage(
                QString(tr("Merge branch %1 into %2"))
                .arg(m_sFromBranch)
                .arg(m_pRepository->currentBranchName())
                );

    return bMergeOK;
}

//-------------------------------------------------------------------------------------------------

}
