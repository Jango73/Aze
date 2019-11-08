
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRevertCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRevertCommand::CRevertCommand(CRepository* pRepository, const QStringList& lFileNames, bool bAllowFileDelete)
    : CBaseCommand(pRepository)
    , m_bAllowFileDelete(bAllowFileDelete)
    , m_pWorkingDirectory(pRepository->commitFunctions()->directoryAsCommit(this))
    , m_lFileNames(lFileNames)
{
}

//-------------------------------------------------------------------------------------------------

CRevertCommand::CRevertCommand(CRepository* pRepository, CCommit* pWorkingDirectory, bool bAllowFileDeleted)
    : CBaseCommand(pRepository)
    , m_bAllowFileDelete(bAllowFileDeleted)
    , m_pWorkingDirectory(pWorkingDirectory)
    , m_lFileNames(pWorkingDirectory->files().values())
{
}

//-------------------------------------------------------------------------------------------------

bool CRevertCommand::execute()
{
    for (QString sFileName : m_lFileNames)
    {
        QString sRelativeFileName = m_pRepository->database()->relativeFileName(sFileName);

        if (not revertSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRevertCommand::revertSingleFile(QString sRelativeFileName)
{
    bool bInStage = false;

    // Check presence of stage
    if (IS_NULL(m_pRepository->stagingCommit()))
    {
        OUT_ERROR(CStrings::s_sTextNoStagingCommit);
        return false;
    }

    bInStage = m_pRepository->stagingCommit()->files().values().contains(sRelativeFileName);

    // Proceed only when there is a tip commit
    if (not IS_NULL(m_pRepository->tipCommit()))
    {
        // Get the id of this file in the tip commit
        QString sIdInTip = mapKeyForValue(m_pRepository->tipCommit()->files(), sRelativeFileName);

        // If file exists in tip, copy it from DB if required
        if (not sIdInTip.isEmpty())
        {
            // Get the id of this file in the working directory
            QString sIdInWorkingDir = mapKeyForValue(m_pWorkingDirectory->files(), sRelativeFileName);

            // Copy the commited file back to the working directory when required
            if (sIdInTip != sIdInWorkingDir)
            {
                QByteArray baData = m_pRepository->database()->getObject(sIdInTip);

                if (not baData.isEmpty())
                {
                    CUtils::putBinaryFileContent(
                                m_pRepository->database()->composeLocalFileName(sRelativeFileName),
                                baData
                                );
                }
            }
        }
        else
        {
            // Delete the commited file from the working directory if not present in the stage
            if (not bInStage && m_bAllowFileDelete)
            {
                QFile tFile(m_pRepository->database()->composeLocalFileName(sRelativeFileName));
                tFile.remove();
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
