
#pragma once

// Global
#include "../Aze_global.h"

// Application
#include "CBaseCommand.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CLogCommand : public CBaseCommand
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------------------------------

    CLogCommand(CRepository* pRepository, const QStringList& lFileNames, QString* pResult, int iStart = 0, int iCount = 0);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual bool execute() override;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QStringList m_lFileNames;
    QString*    m_pResult;
    int         m_iStart;
    int         m_iCount;
};

}
