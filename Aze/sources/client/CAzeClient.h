
#pragma once

// Qt
#include <QObject>
#include <QCoreApplication>

// AzeLib
#include "CRepository.h"

// Application
#include "../common/CConstants.h"
#include "CAzeClientArguments.h"

//-------------------------------------------------------------------------------------------------

class CAzeClient : public QCoreApplication
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CAzeClient(int argc, char *argv[], QTextStream* pOutStream = nullptr);

    //! Destructor
    virtual ~CAzeClient();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    int run();

    //!
    bool isASainRepository();

    //!
    void processWildCards();

    //-------------------------------------------------------------------------------------------------
    // User commands
    //-------------------------------------------------------------------------------------------------

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
    int cleanUp();

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

    CAzeClientArguments     m_tArguments;
    QTextStream*            m_pOutStream;
    Aze::CRepository*       m_pRepository;
    bool                    m_bOutputStreamIsMine;
};

//-------------------------------------------------------------------------------------------------
