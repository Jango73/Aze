
// std
#include <string>
#include <sstream>

// Qt
#include <QDebug>
#include <QDir>

// dtl
#include <dtl.hpp>

// Application
#include "CDiffCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

using dtl::Diff;

//-------------------------------------------------------------------------------------------------

CDiffCommand::CDiffCommand(CRepository* pRepository, const QString& sFirst, const QString& sSecond, QString* pResult)
    : CBaseCommand(pRepository)
    , m_sFirst(sFirst)
    , m_sSecond(sSecond)
    , m_pResult(pResult)
{
}

//-------------------------------------------------------------------------------------------------

bool CDiffCommand::execute()
{
    OUT_DEBUG(QString("Begin"));

    QString sResult;
    QString sObject1 = m_sFirst;
    QString sObject2 = m_sSecond;
    int iDelta1 = 0;
    int iDelta2 = 0;
    bool bWorkOnCommits = false;

    // Check presence of current branch
    if (IS_NULL(m_pRepository->currentBranch()))
    {
        OUT_ERROR(CStrings::s_sTextNoCurrentBranch);
        return false;
    }

    if (sObject1.isEmpty() && sObject2.isEmpty())
    {
        sObject1 = m_pRepository->currentBranch()->rootCommitId();
    }

    sObject1 = m_pRepository->processDeltas(sObject1, iDelta1);
    sObject2 = m_pRepository->processDeltas(sObject2, iDelta2);

    sObject1 = m_pRepository->processKeywords(sObject1);
    sObject2 = m_pRepository->processKeywords(sObject2);

    QString sCommitFileName1 = m_pRepository->database()->composeCommitFileName(sObject1);
    QString sCommitFileName2 = m_pRepository->database()->composeCommitFileName(sObject2);

    OUT_DEBUG(QString("sCommitFileName1: %1").arg(sCommitFileName1));
    OUT_DEBUG(QString("sCommitFileName2: %1").arg(sCommitFileName2));

    if (QFile(sCommitFileName1).exists() && QFile(sCommitFileName2).exists())
    {
        bWorkOnCommits = true;
    }

    if (QFile(sCommitFileName1).exists() && sObject2.isEmpty())
    {
        bWorkOnCommits = true;
    }

    if (QFile(sCommitFileName2).exists() && sObject1.isEmpty())
    {
        bWorkOnCommits = true;
    }

    // The provided object(s) is a(are) commit(s)
    if (bWorkOnCommits)
    {
        CCommit* pCommit1 = nullptr;
        CCommit* pCommit2 = nullptr;

        if (not sObject1.isEmpty() && not sObject2.isEmpty())
        {
            pCommit1 = CCommit::fromFile(sCommitFileName1, this);
            pCommit2 = CCommit::fromFile(sCommitFileName2, this);
        }
        else if (not sObject1.isEmpty() && sObject2.isEmpty())
        {
            if (IS_NULL(m_pRepository->stagingCommit()))
                return false;

            pCommit1 = CCommit::fromFile(sCommitFileName1, this);
            pCommit2 = m_pRepository->stagingCommit()->clone();
        }
        else if (sObject1.isEmpty() && not sObject2.isEmpty())
        {
            if (IS_NULL(m_pRepository->stagingCommit()))
                return false;

            pCommit1 = CCommit::fromFile(sCommitFileName2, this);
            pCommit2 = m_pRepository->stagingCommit()->clone();
        }

        if (not IS_NULL(pCommit1) && not IS_NULL(pCommit2))
        {
            if (iDelta1 != 0)
            {
                pCommit1 = m_pRepository->getCommitAncestor(pCommit1, iDelta1, this);
            }

            if (iDelta2 != 0)
            {
                pCommit2 = m_pRepository->getCommitAncestor(pCommit2, iDelta2, this);
            }

            for (QString sName : pCommit2->files().values())
            {
                QByteArray baContent1 = pCommit1->fileContent(m_pRepository->database(), sName);
                QByteArray baContent2 = pCommit2->fileContent(m_pRepository->database(), sName);

                QString sText1(baContent1);
                QString sText2(baContent2);

                std::vector<std::string> lText1 = CUtils::textToStdStringVector(QString(baContent1));
                std::vector<std::string> lText2 = CUtils::textToStdStringVector(QString(baContent2));

                std::stringstream strStream;

                Diff<std::string> diff(lText1, lText2);
                diff.onHuge();
                diff.compose();
                diff.composeUnifiedHunks();
                diff.printUnifiedFormat(strStream);

                QString sDiffText = QString::fromUtf8(strStream.str().data());

                if (not sDiffText.isEmpty())
                {
                    sResult += QString("diff --aze %1 %2%3")
                            .arg(sName)
                            .arg(sName)
                            .arg(CStrings::s_sNewLine);

                    sResult += sDiffText;
                }
            }
        }
    }

    (*m_pResult) = sResult;

    return true;
}

}
