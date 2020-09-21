
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CStageCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CStageCommand::CStageCommand(CRepository* pRepository, const QStringList& lFileNames)
    : CBaseCommand(pRepository)
    , m_lFileNames(lFileNames)
{
}

//-------------------------------------------------------------------------------------------------

bool CStageCommand::execute()
{
    for (QString sFileName : m_lFileNames)
    {
        QString sRelativeFileName = m_pRepository->database()->relativeFileName(sFileName);

        if (not addSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CStageCommand::addSingleFile(QString sRelativeFileName)
{
    if (not IS_NULL(m_pRepository->stagingCommit()))
    {
        if (CUtils::fileExists(m_pRepository->database()->rootPath(), sRelativeFileName))
        {
            QString sId = CUtils::idFromString(sRelativeFileName);
            m_pRepository->stagingCommit()->addFile(m_pRepository->database(), sRelativeFileName, sId);
            return true;
        }
        else
        {
            m_pRepository->tellError(QString("%1: %2").arg(CStrings::s_sTextNoSuchFile).arg(sRelativeFileName));
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

}
