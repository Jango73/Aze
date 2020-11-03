
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
        QString sRelativeFileName = m_pRepository->database()->relativeFileName(sFileName);

        if (not removeSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRemoveCommand::removeSingleFile(QString sRelativeFileName)
{
    // Sanity check on stage
    if (not m_pRepository->stagingCommit().isNull())
    {
        // Check existence of given file
        if (CUtils::fileExists(m_pRepository->database()->rootPath(), sRelativeFileName))
        {
            // Get the absolute file name
            QString sAbsoluteFileName = m_pRepository->database()->absoluteFileName(sRelativeFileName);

            QFile file(sAbsoluteFileName);

            // If file removal OK, stage the file
            if (file.remove())
            {
                m_pRepository->stagingCommit()->addFile(m_pRepository->database(), sRelativeFileName, "");
                return true;
            }
            else
            {
                m_pRepository->tellError(QString("%1: %2").arg(CStrings::s_sTextNoSuchFile).arg(sRelativeFileName));
            }
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
