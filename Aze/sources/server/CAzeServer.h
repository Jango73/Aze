
#pragma once

// Qt
#include <QObject>
#include <QCoreApplication>
#include <QString>

// qt-plus
#include "Macros.h"

// AzeLib
#include "CRepository.h"

// Application
#include "../common/CConstants.h"
#include "CAzeServerArguments.h"
#include "CAzeServerProcessor.h"

//-------------------------------------------------------------------------------------------------

class CAzeServer : public QCoreApplication
{
    Q_OBJECT

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(QString, s, repositoriesBasePath, RepositoriesBasePath)

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CAzeServer(int argc, char *argv[]);

    //! Destructor
    virtual ~CAzeServer();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CAzeServerArguments     m_tArguments;
    CAzeServerProcessor*    m_pProcessor;
};

//-------------------------------------------------------------------------------------------------
