
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CRevertCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CRevertCommand::CRevertCommand(CRepository* pRepository, const QStringList& lFileNames)
    : CBaseCommand(pRepository)
    , m_pWorkingDirectory(pRepository->workingDirectoryAsCommit(this))
    , m_lFileNames(lFileNames)
{
}

//-------------------------------------------------------------------------------------------------

CRevertCommand::CRevertCommand(CRepository* pRepository, CCommit* pWorkingDirectory)
    : CBaseCommand(pRepository)
    , m_pWorkingDirectory(pWorkingDirectory)
    , m_lFileNames(pWorkingDirectory->files().values())
{
}

//-------------------------------------------------------------------------------------------------

bool CRevertCommand::execute()
{
    for (QString sFileName : m_lFileNames)
    {
        OUT_DEBUG(sFileName);

        QString sRelativeFileName = m_pRepository->database()->relativeFileName(sFileName);

        if (not revertSingleFile(sRelativeFileName))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CRevertCommand::revertSingleFile(QString sRelativeFileName)
{
    if (not IS_NULL(m_pRepository->tipCommit()))
    {
        QString sIdInTip = mapKeyForValue(m_pRepository->tipCommit()->files(), sRelativeFileName);

        if (not sIdInTip.isEmpty())
        {
            if (mapKeyForValue(m_pWorkingDirectory->files(), sRelativeFileName) != sIdInTip)
            {
                // Copy the commited file to the working directory
                QByteArray baData = m_pRepository->database()->getObject(sIdInTip);
                QString sFullFileName = m_pRepository->database()->composeLocalFileName(sRelativeFileName);
                CUtils::putBinaryFileContent(sFullFileName, baData);
            }
        }
        else
        {
            // Delete the commited file from the working directory
            QString sFullFileName = m_pRepository->database()->composeLocalFileName(sRelativeFileName);
            QString sNewFullFileName = QString("%1.bak").arg(sFullFileName);
            QFile tNewFile(sNewFullFileName);
            if (tNewFile.exists())
                tNewFile.remove();
            QFile tFile(sFullFileName);
            tFile.rename(sNewFullFileName);
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

}
