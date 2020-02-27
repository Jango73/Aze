
// Qt
#include <QDebug>
#include <QDir>
#include <QHash>

// Application
#include "CStatusCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CStatusCommand::CStatusCommand(CRepository* pRepository, const QStringList& lFileNames, QList<CFile>* pFileStatus, CEnums::EFileStatus* pGeneralStatus)
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
    , m_pFileStatus(pFileStatus)
    , m_pGeneralStatus(pGeneralStatus)
{
}

//-------------------------------------------------------------------------------------------------

bool CStatusCommand::execute()
{
    QHash<QString, int> lProcessed;

    if (IS_NULL(m_pFileStatus))
        return false;

    if (IS_NULL(m_pGeneralStatus))
        return false;

    if (IS_NULL(m_pRepository->stagingCommit()))
        return false;

    (*m_pGeneralStatus) = CEnums::eClean;

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

    // Get the working directory
    CCommit* pWorkCommit = m_pRepository->commitFunctions()->directoryAsCommit(this);

    QStringList lWorkFiles = pWorkCommit->files().values();
    lWorkFiles.sort();

    // Iterate through working directory files
    // Check how each file differs from the tip commit
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

                (*m_pGeneralStatus) = CEnums::eModified;
            }
        }
        else if (CUtils::idValid(sIdInFrom) && not CUtils::idValid(sIdInWork))
        {
            file.setStatus(CEnums::eDeleted);

            (*m_pGeneralStatus) = CEnums::eModified;
        }
        else
        {
            if (sIdInWork != sIdInFrom)
            {
                file.setStatus(CEnums::eModified);

                (*m_pGeneralStatus) = CEnums::eModified;
            }
            else
            {
                file.setStatus(CEnums::eClean);
            }
        }

        lProcessed[sRelativeName] = 0;
        (*m_pFileStatus) << file;
    }

    // Traverse tip commit files
    // Every file in the tip not existing in working directory is considered missing
    // TODO: Unless marked as deleted
    if (IS_NOT_NULL(m_pRepository->tipCommit()))
    {
        for (QString sRelativeName : m_pRepository->tipCommit()->files().values())
        {
            if (not lProcessed.contains(sRelativeName))
            {
                CFile file;
                file.setRelativeName(sRelativeName);
                file.setStatus(CEnums::eMissing);

                (*m_pGeneralStatus) = CEnums::eModified;

                lProcessed[sRelativeName] = 0;
                (*m_pFileStatus) << file;
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
