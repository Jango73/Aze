
#pragma once

// Qt
#include <QObject>
#include <QCoreApplication>

// AzeLib
#include "CRepository.h"

// Application
#include "CConstants.h"

//-------------------------------------------------------------------------------------------------

class AzeApp : public QCoreApplication
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    AzeApp(int argc, char *argv[]);

    //! Destructor
    virtual ~AzeApp();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    int run();

    //!
    int init();

    //!
    int status();

    //!
    int add();

    //!
    int move();

    //!
    int remove();

    //!
    int commit();

    //!
    bool isASainRepository();

    //!
    void initCommandMap();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QStringList                             m_lArguments;
    QStringList                             m_lSwitches;
    QStringList                             m_lFilesAndIds;
    CConstants::ECommand                    m_eCommand;
    Aze::CRepository*                       m_pRepository;
    QMap<QString, CConstants::ECommand>     m_mCommands;
};
