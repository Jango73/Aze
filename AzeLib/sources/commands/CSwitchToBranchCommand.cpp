
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CSwitchToBranchCommand.h"
#include "CSaveStashCommand.h"
#include "CPopStashCommand.h"
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
    if (m_pRepository->currentBranch().isNull())
    {
        m_pRepository->tellError(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    QString sFileName = m_pRepository->database()->composeBranchFileName(m_sBranchName);

    if (not QFile(sFileName).exists())
        return false;

    // Save stash : diffs between last commit and working directory
    CSaveStashCommand stash(m_pRepository, "");
    stash.setCalledInternally(true);
    bool bStashSaved = stash.execute();

    if (not bStashSaved)
    {
        m_pRepository->tellError(CStrings::s_sTextStashSaveFailed);
        return false;
    }

    // Make the branch switch
    m_pRepository->setCurrentBranchName(m_sBranchName);
    m_pRepository->refreshCurrentBranch();

    // Get the working directory as commit
    CCommit* pToCommit = m_pRepository->commitFunctions()->directoryAsCommit(this);

    // Revert local files to their last committed state
    if (IS_NOT_NULL(pToCommit))
    {
        m_pRepository->revert(pToCommit, m_bAllowFileDelete);
    }

    bool bStashPopped = CPopStashCommand(m_pRepository, "").execute();

    if (not bStashPopped)
    {
        m_pRepository->tellError(CStrings::s_sTextStashPopFailed);
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
