
// Qt
#include <QDebug>

// qt-plus
#include <CXMLNode.h>

// Application
#include "Aze.h"

/*!
    \class Aze
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

Aze::Aze(int argc, char *argv[])
    : QCoreApplication(argc, argv)
    , m_eCommand(CConstants::eCommandNone)
{
    QStringList lArguments;

    for (int Index = 0; Index < argc; Index++)
        lArguments.append(QString(argv[Index]));

    // Get command
    if (lArguments.contains(CConstants::s_sSwitchShowStatus))
    {
        m_eCommand = CConstants::eCommandShowStatus;
    }
}

//-------------------------------------------------------------------------------------------------

Aze::~Aze()
{
}

//-------------------------------------------------------------------------------------------------

int Aze::run()
{
    return 0;
}
