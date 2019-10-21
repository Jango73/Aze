
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRemoveCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRemoveCommand::CRemoveCommand(CRepository* pRepository, const QStringList& lFileNames)
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
{
}

//-------------------------------------------------------------------------------------------------

bool CRemoveCommand::execute()
{
    for (QString sFileName : m_lFileNames)
    {
        QString sRelativeFileName = CUtils::relativeFileName(m_pRepository->rootPath(), sFileName);

        OUT_DEBUG(sRelativeFileName);

        if (not removeSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRemoveCommand::removeSingleFile(QString sRelativeFileName)
{
    OUT_DEBUG(sRelativeFileName);

    if (not IS_NULL(m_pRepository->stagingCommit()))
    {
        if (CUtils::fileExists(m_pRepository->rootPath(), sRelativeFileName))
        {
            QString sId = CUtils::idFromString(sRelativeFileName);
            m_pRepository->stagingCommit()->addFile(sRelativeFileName, sId);
            return true;
        }
        else
        {
            OUT_ERROR(QString("%1: %2").arg(CStrings::s_sTextNoSuchFile).arg(sRelativeFileName));
        }
    }

    return false;
}

}
