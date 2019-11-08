
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
    QList<CCommit*> lFromCommitChain;
    QList<CCommit*> lToCommitChain;

    // Check validity of arguments
    if (m_sFromBranch.isEmpty())
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextBranchNameEmpty);
        return false;
    }

    // Check presence of stage
    if (IS_NULL(m_pRepository->stagingCommit()))
    {
        OUT_ERROR(CStrings::s_sTextNoStagingCommit);
        return false;
    }

    // Check presence of current branch tip commit
    if (IS_NULL(m_pRepository->tipCommit()))
    {
        OUT_ERROR(CStrings::s_sTextNoTipCommit);
        return false;
    }

    CCommit* pToTipCommit = m_pRepository->tipCommit();

    CBranch* pFromBranch = m_pRepository->database()->getBranch(m_sFromBranch, this);

    // Check presence of 'from' branch tip commit
    if (IS_NULL(pFromBranch) || pFromBranch->tipCommitId().isEmpty())
    {
        OUT_ERROR(CStrings::s_sTextNoSuchBranch);
        return false;
    }

    //
    if (pFromBranch->tipCommitId() == m_pRepository->tipCommit()->id())
    {
        OUT_ERROR(CStrings::s_sTextCannotMergeSameCommits);
        return false;
    }

    CCommit* pFromTipCommit = m_pRepository->database()->getCommit(pFromBranch->tipCommitId(), this);

    // Check presence of 'from' branch tip commit
    if (IS_NULL(pFromTipCommit))
    {
        OUT_ERROR(CStrings::s_sTextNoSuchBranch);
        return false;
    }

    // Get the common commit chains of the two commits
    // Lists are from root to tip
    CCommit* pCommonAncestor = m_pRepository->commitFunctions()->getCommonCommitChains(
                pToTipCommit,
                pFromTipCommit,
                this,
                &lToCommitChain,
                &lFromCommitChain
                );

    // Check presence of common ancestor
    if (IS_NULL(pCommonAncestor))
    {
        OUT_ERROR(CStrings::s_sTextNoCommonAncestor);
        return false;
    }

    // Check relevancy of 'from' commit
    if (pCommonAncestor == pFromTipCommit)
    {
        OUT_ERROR(CStrings::s_sTextNoCommitToMerge);
        return false;
    }

    // Check status of working directory
    m_pRepository->checkStatus(QStringList());

    if (m_pRepository->status() != CEnums::eClean)
    {
        OUT_ERROR(CStrings::s_sTextMergeWorkingDirectoryNotClean);
        return false;
    }

    // Get a diff between the two commits
    QString sDiff;
    m_pRepository->commitFunctions()->diffCommitLists(sDiff, lToCommitChain, lFromCommitChain);

    if (sDiff.isEmpty())
    {
        OUT_ERROR("Diff is empty.");
        return false;
    }

    if (not m_pRepository->applyDiff(sDiff, true))
    {
        OUT_ERROR(CStrings::s_sTextMergeFailed);
        return false;
    }

    // Set merge information in staging commit
    // Only the 'from' commit is added as a parent here, the other is added on commit
    m_pRepository->stagingCommit()->setIsMerge(true);
    m_pRepository->stagingCommit()->clearParents();
    m_pRepository->stagingCommit()->addParent(pFromTipCommit->id());
    m_pRepository->stagingCommit()->setMessage(
                QString(tr("Merge branch %1 into %2"))
                .arg(m_sFromBranch)
                .arg(m_pRepository->currentBranchName())
                );

    return true;
}

//-------------------------------------------------------------------------------------------------

}
