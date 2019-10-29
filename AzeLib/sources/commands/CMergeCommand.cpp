
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CMergeCommand.h"
#include "../CRepository.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CMergeCommand::CMergeCommand(CRepository* pRepository, const QString& sFrom)
    : CBaseCommand(pRepository)
    , m_sFrom(sFrom)
{
}

//-------------------------------------------------------------------------------------------------

bool CMergeCommand::execute()
{
    // Check validity of arguments
    if (m_sFrom.isEmpty())
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

    // Check presence of current branch tip commit
    if (IS_NULL(m_pRepository->tipCommit()))
    {
        OUT_ERROR(CStrings::s_sTextNoTipCommit);
        return false;
    }

    CBranch* pFromBranch = m_pRepository->database()->getBranch(m_sFrom, this);

    // Check presence of 'from' branch tip commit
    if (IS_NULL(pFromBranch) || pFromBranch->tipCommitId().isEmpty())
    {
        OUT_ERROR(CStrings::s_sTextNoSuchBranch);
        return false;
    }

    CCommit* pFromTipCommit = m_pRepository->database()->getCommit(pFromBranch->tipCommitId(), this);

    // Check presence of 'from' branch tip commit
    if (IS_NULL(pFromTipCommit))
    {
        OUT_ERROR(CStrings::s_sTextNoSuchBranch);
        return false;
    }

    CCommit* pCommonAncestor = m_pRepository->getCommitsCommonAncestor(
                m_pRepository->tipCommit(),
                pFromTipCommit,
                this
                );

    // Check presence of common ancestor
    if (IS_NULL(pCommonAncestor))
    {
        OUT_ERROR(CStrings::s_sTextNoCommonAncestor);
        return false;
    }

    OUT_DEBUG(pCommonAncestor->id());

    return true;
}

//-------------------------------------------------------------------------------------------------

}
