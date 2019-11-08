
#pragma once

// Qt
#include <QObject>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

// AzeLib
#include "CRepository.h"

// Application
#include "CConstants.h"

//-------------------------------------------------------------------------------------------------

class CAzeArguments : public QObject
{
public:

    CAzeArguments(QCoreApplication& app);

    QCommandLineParser      m_tParser;
    QCommandLineOption      m_oAll;
    QCommandLineOption      m_oLoose;
    QCommandLineOption      m_oClean;
    QCommandLineOption      m_oModified;
    QCommandLineOption      m_oAdded;
    QCommandLineOption      m_oDeleted;
    QCommandLineOption      m_oMissing;
    QCommandLineOption      m_oIgnored;
    QCommandLineOption      m_oAuthor;
    QCommandLineOption      m_oMessage;
    QCommandLineOption      m_oStart;
    QCommandLineOption      m_oCount;
    QCommandLineOption      m_oAllowFileDelete;
    QCommandLineOption      m_oGraph;
    CConstants::ECommand    m_eCommand;
    QStringList             m_lFilesAndIds;
};

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

    CAzeArguments           m_tArguments;
    QTextStream*            m_pOutStream;
    Aze::CRepository*       m_pRepository;
};
