
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRemoteRepository.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRemoteRepository::CRemoteRepository(CRepository* pLocalRepository, QObject* parent)
    : CObject(parent)
    , m_pLocalRepository(pLocalRepository)
{
}

//-------------------------------------------------------------------------------------------------

CRemoteRepository::~CRemoteRepository()
{
}

//-------------------------------------------------------------------------------------------------

CXMLNode CRemoteRepository::getPullRequest()
{
    CXMLNode xRequest("");

    QVector<CBranch*> lBranches = m_pLocalRepository->database()->getAllBranches(this);

    xRequest.attributes()[CStrings::s_sParamRequest] = CStrings::s_sParamPull;

    for (CBranch* pBranch : lBranches)
    {
        CXMLNode xBranch = pBranch->toNode();
        xRequest << xBranch;
    }

    return xRequest;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CRemoteRepository::getPushRequest()
{
    CXMLNode xRequest("");

    xRequest.attributes()[CStrings::s_sParamRequest] = CStrings::s_sParamPush;
    xRequest.attributes()[CStrings::s_sParamBranch] = m_pLocalRepository->currentBranchName();

    QList<QPair<int, QString>> lCommitList = m_pLocalRepository->commitFunctions()->getCommitAncestorList(
                m_pLocalRepository->tipCommit()->id(),
                true,
                0
                );

    QStringList lCommmitIds;
    for (QPair<int, QString> pCommit : lCommitList)
        lCommmitIds << pCommit.second;

    QList<CCommit*> lCommits  =CCommit::fromIdList(m_pLocalRepository->database(), lCommmitIds, nullptr);
    reverseList(lCommits);

    xRequest << m_pLocalRepository->tipCommit()->toNode();

    for (CCommit* pCommit : lCommits)
        xRequest << pCommit->toNode();

    qDeleteAll(lCommits);

    return xRequest;
}

//-------------------------------------------------------------------------------------------------

}
