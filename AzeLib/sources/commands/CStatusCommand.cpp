
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

    if (IS_NULL(m_pRepository->tipCommit()))
        return false;

    //    for (QString sFileName : m_lFileNames)
    //    {
    //        QString sRelativeFileName = m_pRepository->database()->relativeFileName(sFileName);

    //        OUT_DEBUG(sRelativeFileName);
    //    }

    QList<CFile> lReturnValue;

    CCommit* pWorkCommit = m_pRepository->workingDirectoryAsCommit(this);
    CCommit* pFromCommit = m_pRepository->tipCommit();

    for (QString sIdInWork : pWorkCommit->files().keys())
    {
        QString sRelativeName = pWorkCommit->files()[sIdInWork];

        CFile file;
        file.setRelativeName(sRelativeName);

        QString sIdInFrom = mapKeyForValue(pFromCommit->files(), sRelativeName);

        if (sIdInFrom.isEmpty())
        {
            file.setStatus(CEnums::eAdded);
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

    for (QString sIdInFrom : pFromCommit->files().keys())
    {
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
