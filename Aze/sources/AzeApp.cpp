
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
    CConstants::initCommandMap();

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
    m_eCommand = CConstants::s_mCommands[m_lArguments.takeFirst().toLower()];

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

QString AzeApp::getArgumentValue(const QString& sName)
{
    for (int iIndex = 0; iIndex < m_lArguments.count(); iIndex++)
    {
        if (m_lArguments[iIndex] == sName)
        {
            m_lArguments.takeAt(iIndex);

            if (iIndex < m_lArguments.count())
                return m_lArguments.takeAt(iIndex);

            return "";
        }
    }

    return "";
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

    case CConstants::eCommandCreateBranch:
        return createBranch();

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

    case CConstants::eCommandDump:
        return dump();
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::init()
{
    return m_pRepository->init() ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::createBranch()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_lFilesAndIds.count() > 0, CConstants::s_iError_NoBranchNameGiven);

    ERROR_WHEN_FALSE(m_pRepository->createBranch(m_lFilesAndIds[0]), CConstants::s_iError_CouldNotCreateBranch);

    ERROR_WHEN_FALSE(m_pRepository->writeCurrentBranch(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->writeGeneralInfo(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::status()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    QList<Aze::CFile> lFiles = m_pRepository->fileStatus();
    return 0;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::add()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_pRepository->add(m_lFilesAndIds), CConstants::s_iError_CouldNotAddFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::move()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::remove()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_pRepository->remove(m_lFilesAndIds), CConstants::s_iError_CouldNotRemoveFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::commit()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    QString sAuthor = getArgumentValue(CConstants::s_sSwitchAuthor);
    QString sMessage = getArgumentValue(CConstants::s_sSwitchMessage);

    ERROR_WHEN_FALSE(m_pRepository->commit(sAuthor, sMessage), CConstants::s_iError_CouldNotRemoveFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeCurrentBranch(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->clearStage(), CConstants::s_iError_CouldNotWriteStage);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::dump()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    for (QString sId : m_lFilesAndIds)
    {
        QString sText = m_pRepository->getFileContent(sId);

        std::printf("%s", sText.toLatin1().constData());
    }

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

bool AzeApp::isASainRepository()
{
    if (not m_pRepository->ok())
    {
        OUT_ERROR("This is not an Aze repository.");

        return false;
    }

    return true;
}
