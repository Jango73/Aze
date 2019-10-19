
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
    int run();

    //! Initializes a repository
    int init();

    //!
    int createBranch();

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
    int diff();

    //!
    int dump();

    //!
    bool isASainRepository();

    //!
    void processWildCards();

    //!
    void processWildCardsRecurse(const QString& sCurrentDirectory, const QString& sWildCard);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QStringList                             m_lArguments;
    QStringList                             m_lSwitches;
    QStringList                             m_lFilesAndIds;
    CConstants::ECommand                    m_eCommand;
    QTextStream*                            m_pOutStream;
    Aze::CRepository*                       m_pRepository;
};
