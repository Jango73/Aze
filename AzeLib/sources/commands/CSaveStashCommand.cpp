
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CSaveStashCommand.h"
#include "CRevertCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CSaveStashCommand::CSaveStashCommand(CRepository* pRepository, const QString& sMessage)
    : CBaseCommand(pRepository)
    , m_sMessage(sMessage)
{
}

//-------------------------------------------------------------------------------------------------

bool CSaveStashCommand::execute()
{
    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        if (not m_bCalledInternally)
            m_pRepository->tellError(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    // Check presence of staging commit
    if (m_pRepository->stagingCommit().isNull())
    {
        if (not m_bCalledInternally)
            m_pRepository->tellError(CStrings::s_sTextNoStagingCommit);
        return false;
    }

    // Get diff of working directory
    QString sDiff = m_pRepository->diffWorkingDirectory();

    if (sDiff.isEmpty())
    {
        if (not m_bCalledInternally)
            m_pRepository->tellInfo(CStrings::s_sTextNothingToStash);
        return true;
    }

    // Create a new stash object
    CStash* pNewStash = new CStash(this);

    pNewStash->setDateToNow();
    pNewStash->setAuthor(m_sAuthor);
    pNewStash->setMessage(m_sMessage);
    pNewStash->setDiff(sDiff);
    pNewStash->setStage(m_pRepository->stagingCommit()->clone(pNewStash));

    QString sNewStashId = m_pRepository->database()->storeStash(pNewStash);
    m_pRepository->addStashToList(sNewStashId);

    m_pRepository->clearStage();

    CCommit* pWorkingDirectory = m_pRepository->commitFunctions()->directoryAsCommit(this);
    CRevertCommand(m_pRepository, pWorkingDirectory, true).execute();

    return not sNewStashId.isEmpty();
}

//-------------------------------------------------------------------------------------------------

}
