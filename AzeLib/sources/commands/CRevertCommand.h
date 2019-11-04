
#pragma once

// Global
#include "../Aze_global.h"

// Application
#include "CBaseCommand.h"
#include "../objects/CCommit.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CRevertCommand : public CBaseCommand
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------------------------------

    //!
    CRevertCommand(CRepository* pRepository, const QStringList& lFileNames);

    //!
    CRevertCommand(CRepository* pRepository, CCommit* pWorkingDirectory);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual bool execute() override;

    //!
    bool revertSingleFile(QString sRelativeFileName);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CCommit*        m_pWorkingDirectory;
    QStringList     m_lFileNames;
};

}
