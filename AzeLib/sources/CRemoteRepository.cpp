
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

    QList<CCommit*> lCommitList = m_pLocalRepository->commitFunctions()->getCommitAncestorList(
                m_pLocalRepository->tipCommit(),
                this,
                true,
                0,
                nullptr
                );

    REVERSE_LIST(lCommitList);

    xRequest << m_pLocalRepository->tipCommit()->toNode();

    for (CCommit* pCommit : lCommitList)
        xRequest << pCommit->toNode();

    return xRequest;
}

//-------------------------------------------------------------------------------------------------

}
