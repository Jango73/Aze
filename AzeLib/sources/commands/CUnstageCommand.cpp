
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CUnstageCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CUnstageCommand::CUnstageCommand(CRepository* pRepository, const QStringList& lFileNames)
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
{
}

//-------------------------------------------------------------------------------------------------

bool CUnstageCommand::execute()
{
    for (QString sFileName : m_lFileNames)
    {
        QString sRelativeFileName = m_pRepository->database()->relativeFileName(sFileName);

        if (not unstageSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CUnstageCommand::unstageSingleFile(QString sRelativeFileName)
{
    if (not m_pRepository->stagingCommit().isNull())
    {
        m_pRepository->stagingCommit()->removeFile(sRelativeFileName);
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

}
