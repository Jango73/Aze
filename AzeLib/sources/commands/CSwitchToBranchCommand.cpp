
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
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    QString sFileName = m_pRepository->database()->composeBranchFileName(m_sBranchName);

    if (not QFile(sFileName).exists())
        return false;

    // Save stash : diffs between last commit and working directory
    bool bStashSaved = CSaveStashCommand(m_pRepository, "").execute();

    if (not bStashSaved)
    {
        OUT_ERROR(CStrings::s_sTextStashSaveFailed);
        return false;
    }

    // Make the branch switch
    m_pRepository->setCurrentBranchName(m_sBranchName);
    m_pRepository->readCurrentBranch();
    m_pRepository->readTipCommit();

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
        OUT_ERROR(CStrings::s_sTextStashPopFailed);
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
