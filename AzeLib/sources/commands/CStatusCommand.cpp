
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CStatusCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CStatusCommand::CStatusCommand(CRepository* pRepository, const QStringList& lFileNames, QList<CFile>* pResult)
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
    , m_pResult(pResult)
{
}

//-------------------------------------------------------------------------------------------------

bool CStatusCommand::execute()
{
    if (IS_NULL(m_pResult))
        return false;

    if (IS_NULL(m_pRepository->stagingCommit()))
        return false;

    QList<CFile> lReturnValue;

    CCommit* pStageCommit = m_pRepository->stagingCommit();
    CCommit* pFromCommit = nullptr;

    if (not IS_NULL(m_pRepository->tipCommit()))
    {
        pFromCommit = m_pRepository->tipCommit();
    }
    else
    {
        pFromCommit = new CCommit(this);
    }

    CCommit* pWorkCommit = m_pRepository->workingDirectoryAsCommit(this);

    QStringList lWorkFiles = pWorkCommit->files().values();
    lWorkFiles.sort();

    for (QString sRelativeName : lWorkFiles)
    {
        QString sIdInFrom = mapKeyForValue(pFromCommit->files(), sRelativeName);
        QString sIdInWork = mapKeyForValue(pWorkCommit->files(), sRelativeName);
        QString sIdInStage = mapKeyForValue(pStageCommit->files(), sRelativeName);

        CFile file;
        file.setRelativeName(sRelativeName);

        if (not CUtils::idValid(sIdInFrom) && not CUtils::idValid(sIdInWork))
        {
            file.setStatus(CEnums::eLoose);
        }
        else if (not CUtils::idValid(sIdInFrom) && CUtils::idValid(sIdInWork))
        {
            if (CUtils::idValid(sIdInStage))
            {
                file.setStatus(CEnums::eAdded);
            }
        }
        else if (CUtils::idValid(sIdInFrom) && not CUtils::idValid(sIdInWork))
        {
            file.setStatus(CEnums::eDeleted);
        }
        else
        {
            if (sIdInWork != sIdInFrom)
            {
                file.setStatus(CEnums::eModified);
            }
            else
            {
                file.setStatus(CEnums::eClean);
            }
        }

        (*m_pResult) << file;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
