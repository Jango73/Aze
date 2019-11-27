
// std
#include <string>
#include <sstream>

// Qt
#include <QDebug>
#include <QDir>
#include <QHash>

// Application
#include "CLogCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CLogCommand::CLogCommand(
        CRepository* pRepository
        , const QStringList& lFileNames
        , QString* pResult
        , bool bGraph
        , int iStart
        , int iCount
        )
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
    , m_pResult(pResult)
    , m_bGraph(bGraph)
    , m_iStart(iStart)
    , m_iCount(iCount)
{
}

//-------------------------------------------------------------------------------------------------

bool CLogCommand::execute()
{
    bool bGetAllLog = m_iCount == 0;

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

    // Keep track of shown commits
    QHash<QString, int> lProcessed;

    // Get the starting commit
    m_mBranches[0] = m_pRepository->tipCommit()->clone(this);

    // We stop when no more branches to show
    while (true)
    {
        if (m_iStart == 0)
        {
            // Iterate through each branch
            for (int index = 0; index < m_mBranches.count(); index++)
            {
                QString sId = m_mBranches[index]->id();

                // Proceed if sId has not been processed yet
                if (not lProcessed.contains(sId))
                {
                    CCommit* pCommit = m_mBranches[index];

                    lProcessed[sId] = 0;

                    if (m_bGraph)
                    {
                        outputBranches(index);

                        (*m_pResult) += QString("%1 - %2 - %3 - %4%5")
                                .arg(pCommit->id())
                                .arg(PRINTABLE_ISO_DATE(m_mBranches[index]->date()))
                                .arg(pCommit->author())
                                .arg(pCommit->message())
                                .arg(CStrings::s_sNewLine);
                    }
                    else
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
                    }
                }
            }

            if (not bGetAllLog)
            {
                m_iCount--;
                if (m_iCount == 0)
                    break;
            }
        }
        else
        {
            m_iStart--;
        }

        QMap<int, CCommit*> mNewBranches;
        int iNewBranchIndex = 0;

        for (int branch = 0; branch < m_mBranches.count(); branch++)
        {
            QList<CCommit*> lParents = CCommit::parentList(
                        m_pRepository->database(), m_mBranches[branch], this
                        );

            for (CCommit* pCommit : lParents)
            {
                mNewBranches[iNewBranchIndex++] = pCommit;
            }
        }

        qDeleteAll(m_mBranches.values());
        m_mBranches.clear();
        m_mBranches = mNewBranches;

        if (m_mBranches.count() == 0)
            break;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

void CLogCommand::outputBranches(int iCurrentBranch)
{
    for (int index = 0; index < m_mBranches.count(); index++)
    {
        if (index == iCurrentBranch)
            (*m_pResult) += QString("*  ");
        else
            (*m_pResult) += QString("|  ");
    }
}

//-------------------------------------------------------------------------------------------------

}
