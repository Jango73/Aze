
#pragma once

// Global
#include "../Aze_global.h"

// Application
#include "CBaseCommand.h"
#include "../objects/CCommit.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CSwitchToBranchCommand : public CBaseCommand
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------------------------------

    CSwitchToBranchCommand(CRepository* pRepository, const QString& sBranchName);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual bool execute() override;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString     m_sBranchName;
};

}
