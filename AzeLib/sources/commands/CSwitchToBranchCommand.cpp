
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CSwitchToBranchCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CSwitchToBranchCommand::CSwitchToBranchCommand(CRepository* pRepository, const QString& sBranchName, bool bAllowFileDelete)
    : CBaseCommand(pRepository)
    , m_bAllowFileDelete(bAllowFileDelete)
    , m_sBranchName(sBranchName)
{
}

//-------------------------------------------------------------------------------------------------

bool CSwitchToBranchCommand::execute()
{
    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    QString sFileName = m_pRepository->database()->composeBranchFileName(m_sBranchName);

    if (not QFile(sFileName).exists())
        return false;

    QStringList lLooseFiles = m_pRepository->getLooseFiles();

    // Save stash : diffs between last commit and working directory
    QString sDiff;
    CCommit* pFromCommit = nullptr;
    CCommit* pToCommit = nullptr;

    if (not m_pRepository->currentBranch()->tipCommitId().isEmpty())
    {
        // Get the current branch tip commit
        pFromCommit = m_pRepository->database()->getCommit(m_pRepository->currentBranch()->tipCommitId(), this);

        // Get the working directory as commit
        pToCommit = m_pRepository->commitFunctions()->directoryAsCommit(this);

        // Diff the current tip commit and the working directory
        m_pRepository->commitFunctions()->diffCommits(sDiff, pFromCommit, pToCommit, lLooseFiles);
    }

    // Make the branch switch
    m_pRepository->setCurrentBranchName(m_sBranchName);
    m_pRepository->readCurrentBranch();
    m_pRepository->readTipCommit();

    // Revert local files to their last committed state
    if (IS_NOT_NULL(pToCommit))
    {
        m_pRepository->revert(pToCommit, m_bAllowFileDelete);
    }

    // Apply the stash
    if (not sDiff.isEmpty())
        return m_pRepository->applyDiff(sDiff);

    return true;
}

//-------------------------------------------------------------------------------------------------

}
