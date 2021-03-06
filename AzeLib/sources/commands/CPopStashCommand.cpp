
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CPopStashCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CPopStashCommand::CPopStashCommand(CRepository* pRepository, const QString& sId)
    : CBaseCommand(pRepository)
    , m_sId(sId)
{
}

//-------------------------------------------------------------------------------------------------

bool CPopStashCommand::execute()
{
    if (m_sId.isEmpty() && m_pRepository->stashList().count() > 0)
        m_sId = m_pRepository->stashList().last();

    if (m_sId.isEmpty())
        return true;

    CStash* pStash = m_pRepository->database()->getStash(m_sId, this);

    if (IS_NOT_NULL(pStash) && not pStash->diff().isEmpty())
    {
        // Apply the stash
        bool ok = m_pRepository->commitFunctions()->applyDiff(pStash->diff());

        if (ok)
        {
            m_pRepository->database()->removeStash(m_sId);
            m_pRepository->removeStashFromList(m_sId);

            if (not m_pRepository->stagingCommit().isNull() && IS_NOT_NULL(pStash->stage()))
            {
                m_pRepository->stagingCommit()->addCommit(m_pRepository->database(), pStash->stage());
            }

            return true;
        }
        else
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
