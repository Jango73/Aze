
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
    QString getArgumentValue(const QString& sName);

    //!
    bool getSwitch(const QString& sName);

    //!
    bool checkRemainingSwitches();

    //!
    int run();

    //!
    bool isASainRepository();

    //!
    void processWildCards();

    //-------------------------------------------------------------------------------------------------
    // User commands
    //-------------------------------------------------------------------------------------------------

    //! Runs Aze unit tests
    int test();

    //! Initializes a repository
    int init();

    //!
    int createBranch();

    //!
    int switchToBranch();

    //!
    int status();

    //!
    int stage();

    //!
    int unstage();

    //!
    int revert();

    //!
    int move();

    //!
    int remove();

    //!
    int commit();

    //!
    int log();

    //!
    int diff();

    //!
    int merge();

    //!
    int dump();

    //!
    int help();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QStringList             m_lArguments;
    QStringList             m_lSwitches;
    QStringList             m_lFilesAndIds;
    CConstants::ECommand    m_eCommand;
    QTextStream*            m_pOutStream;
    Aze::CRepository*       m_pRepository;
};
