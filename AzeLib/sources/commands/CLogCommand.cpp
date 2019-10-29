
// std
#include <string>
#include <sstream>

// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CLogCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CLogCommand::CLogCommand(CRepository* pRepository, const QStringList& lFileNames, QString* pResult)
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
    , m_pResult(pResult)
{
}

//-------------------------------------------------------------------------------------------------

bool CLogCommand::execute()
{
    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    // Check presence of tip commit
    if (IS_NULL(m_pRepository->tipCommit()))
    {
        OUT_ERROR(CStrings::s_sTextNoTipCommit);
        return false;
    }

    CCommit* pCommit = m_pRepository->tipCommit();

    while (pCommit != nullptr)
    {
        (*m_pResult) += QString("commit: %1%2date: %3%4author: %5%6%7%8%9%10")
                .arg(pCommit->id())
                .arg(CStrings::s_sNewLine)
                .arg(pCommit->date())
                .arg(CStrings::s_sNewLine)
                .arg(pCommit->author())
                .arg(CStrings::s_sNewLine)
                .arg(CStrings::s_sNewLine)
                .arg(pCommit->message())
                .arg(CStrings::s_sNewLine)
                .arg(CStrings::s_sNewLine);

        pCommit = m_pRepository->getCommitAncestor(pCommit, this);
    }

    return true;
}

}