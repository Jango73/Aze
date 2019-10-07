
// Qt
#include <QDebug>
#include <QDir>

// qt-plus
#include <CXMLNode.h>

// Application
#include "AzeApp.h"

/*!
    \class AzeApp
    \inmodule AzeApp
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

AzeApp::AzeApp(int argc, char *argv[])
    : QCoreApplication(argc, argv)
    , m_eCommand(CConstants::eCommandNone)
    , m_pRepository(new Aze::CRepository(QDir::currentPath(), this))
{
    QStringList lRawArguments;

    for (int Index = 0; Index < argc; Index++)
        lRawArguments << QString(argv[Index]);

    // Get some arguments from console if none provided
    if (lRawArguments.count() < 2)
    {
        QTextStream out(stdout);
        out << QString("Please enter a command (%1)\n>").arg(m_pRepository->rootPath());
        out.flush();

        QTextStream in(stdin);
        QString value = in.readLine();

        // Get arguments by splitting on space except for quoted strings
        lRawArguments << value.split(QRegExp(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)"));
    }

    // Remove quotes when needed
    for (QString sArg : lRawArguments)
    {
        if (sArg.startsWith("\"") && sArg.endsWith("\""))
        {
            sArg.remove(0, 1);
            sArg.remove(sArg.length() - 1, 1);
        }

        m_lArguments << sArg;
    }

    // Remove executable name
    m_lArguments.takeFirst();

    // Get command
    QString sCommand = m_lArguments.takeFirst().toLower();

    if (sCommand == CConstants::s_sSwitchInitRepository)
        m_eCommand = CConstants::eCommandInitRepository;
    else if (sCommand == CConstants::s_sSwitchStatus)
        m_eCommand = CConstants::eCommandShowStatus;
    else if (sCommand == CConstants::s_sSwitchAdd)
        m_eCommand = CConstants::eCommandAdd;
    else if (sCommand == CConstants::s_sSwitchCommit)
        m_eCommand = CConstants::eCommandCommit;

    // Split switches and files
    for (QString sArgument : m_lArguments)
    {
        m_lFilesAndIds << sArgument;
    }
}

//-------------------------------------------------------------------------------------------------

AzeApp::~AzeApp()
{
}

//-------------------------------------------------------------------------------------------------

int AzeApp::run()
{
    switch (m_eCommand)
    {
    case CConstants::eCommandNone:
        break;

    case CConstants::eCommandInitRepository:
        return init();

    case CConstants::eCommandShowStatus:
        return status();

    case CConstants::eCommandAdd:
        return add();

    case CConstants::eCommandMove:
        return move();

    case CConstants::eCommandRemove:
        return remove();

    case CConstants::eCommandCommit:
        return commit();
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::init()
{
    qDebug() << "init()";

    return m_pRepository->init() ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::status()
{
    if (not isASainRepository())
        return 1;

    qDebug() << "status()";

    QList<Aze::CFile> lFiles = m_pRepository->fileStatus();
    return 0;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::add()
{
    if (not isASainRepository())
        return 1;

    qDebug() << "add()";

    return m_pRepository->add(m_lFilesAndIds) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::move()
{
    if (not isASainRepository())
        return 1;

    qDebug() << "move()";

    return 0;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::remove()
{
    if (not isASainRepository())
        return 1;

    qDebug() << "remove()";

    return m_pRepository->remove(m_lFilesAndIds) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::commit()
{
    if (not isASainRepository())
        return 1;

    qDebug() << "commit()";

    return 0;
}

//-------------------------------------------------------------------------------------------------

bool AzeApp::isASainRepository()
{
    if (not m_pRepository->ok())
    {
        qWarning() << "This is not an Aze repository.";
        return false;
    }

    return true;
}
