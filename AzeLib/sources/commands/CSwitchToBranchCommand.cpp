
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CSwitchToBranchCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CSwitchToBranchCommand::CSwitchToBranchCommand(CRepository* pRepository, const QString& sBranchName)
    : CBaseCommand(pRepository)
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

    // Get diffs between last commit and working directory
    QString sDiff;
    CCommit* pFromCommit = nullptr;
    CCommit* pToCommit = nullptr;

    if (not m_pRepository->currentBranch()->tipCommitId().isEmpty())
    {
        pFromCommit = m_pRepository->database()->getCommit(m_pRepository->currentBranch()->tipCommitId(), this);
        pToCommit = m_pRepository->workingDirectoryAsCommit(this);

        m_pRepository->diffCommits(sDiff, pFromCommit, pToCommit);
    }

    // Make the branch switch
    m_pRepository->setCurrentBranchName(m_sBranchName);
    m_pRepository->readCurrentBranch();
    m_pRepository->readTipCommit();

    // Revert files to their last committed state
    if (IS_NOT_NULL(pToCommit))
    {
        m_pRepository->revert(pToCommit);
    }

    // Apply the diff
    if (not sDiff.isEmpty())
        m_pRepository->applyDiff(sDiff);

    return true;
}

//-------------------------------------------------------------------------------------------------

}
