
// Qt
#include <QDebug>
#include <QDir>

// Application
#include "CPatchCommand.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CPatchCommand::CPatchCommand(CRepository* pRepository, const QString& sFilename)
    : CBaseCommand(pRepository)
    , m_sFilename(sFilename)
{
}

//-------------------------------------------------------------------------------------------------

bool CPatchCommand::execute()
{
    if (m_sFilename.isEmpty())
        return false;

    QString sDiff = CUtils::getTextFileContent(m_sFilename);

    if (sDiff.isEmpty())
        return false;

    // Apply the patchs
    return m_pRepository->commitFunctions()->applyDiff(sDiff);
}

//-------------------------------------------------------------------------------------------------

}
