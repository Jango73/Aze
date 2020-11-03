
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
    // Sanity check on stage
    if (m_pRepository->stagingCommit().isNull())
    {
        m_pRepository->tellError(CStrings::s_sTextNoStagingCommit);
        return false;
    }

    if (CUtils::fileExists(m_pRepository->database()->rootPath(), sRelativeFileName))
    {
        QString sId = CUtils::idFromString(sRelativeFileName);
        m_pRepository->stagingCommit()->addFile(m_pRepository->database(), sRelativeFileName, sId);
        return true;
    }
    else
    {
        // Check if this is a file deletion
        if (not m_pRepository->tipCommit().isNull() && m_pRepository->tipCommit()->files().contains(sRelativeFileName))
        {
            m_pRepository->stagingCommit()->addFile(m_pRepository->database(), sRelativeFileName, "");
            return true;
        }

        m_pRepository->tellError(QString("%1: %2").arg(CStrings::s_sTextNoSuchFile).arg(sRelativeFileName));
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

}
