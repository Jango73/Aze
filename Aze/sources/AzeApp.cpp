
// Qt
#include <QDebug>
#include <QDir>
#include <QDirIterator>

// qt-plus
#include <CXMLNode.h>
#include <File/CFileUtilities.h>

// Aze
#include "tests/CTestAze.h"

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
    , m_pOutStream(new QTextStream(stdout))
    , m_pRepository(new Aze::CRepository(QDir::currentPath(), this))
{
    CConstants::initCommandMap();

    // Prepare arguments
    QStringList lRawArguments;

    for (int Index = 1; Index < argc; Index++)
    {
        lRawArguments << QString(argv[Index]);
    }

    QString lConcatenatedArguments = lRawArguments.join(" ");

    // Get some arguments from console if none provided
    if (lRawArguments.count() < 1)
    {
        (*m_pOutStream) << QString("Please enter a command (%1)\n>").arg(m_pRepository->database()->rootPath());
        m_pOutStream->flush();

        QTextStream in(stdin);
        lConcatenatedArguments = in.readLine();
    }

    // Get arguments by splitting on space except for quoted strings
    lRawArguments = lConcatenatedArguments.split(QRegExp(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)"));

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

    // Get command
    if (m_lArguments.count() > 0)
    {
        QString sCommand = m_lArguments.takeFirst().toLower();
        m_eCommand = CConstants::s_mCommands[sCommand];
    }

    // Split switches and files
    for (QString sArgument : m_lArguments)
    {
        m_lFilesAndIds << sArgument;
    }
}

//-------------------------------------------------------------------------------------------------

AzeApp::~AzeApp()
{
    delete m_pOutStream;
    delete m_pRepository;
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

bool AzeApp::getSwitch(const QString& sName)
{
    for (int iIndex = 0; iIndex < m_lArguments.count(); iIndex++)
    {
        if (m_lArguments[iIndex] == sName)
        {
            m_lArguments.takeAt(iIndex);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool AzeApp::checkRemainingSwitches()
{
    if (m_lArguments.count() > 0)
    {
        OUT_ERROR(QString(tr("Unknown argument: %1")).arg(m_lArguments[0]));
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::run()
{
    switch (m_eCommand)
    {
    case CConstants::eCommandNone:
        break;

    case CConstants::eCommandRunTests:
        return test();

    case CConstants::eCommandInitRepository:
        return init();

    case CConstants::eCommandCreateBranch:
        return createBranch();

    case CConstants::eCommandSwitchToBranch:
        return switchToBranch();

    case CConstants::eCommandShowStatus:
        return status();

    case CConstants::eCommandStage:
        return stage();

    case CConstants::eCommandUnstage:
        return unstage();

    case CConstants::eCommandMove:
        return move();

    case CConstants::eCommandRemove:
        return remove();

    case CConstants::eCommandCommit:
        return commit();

    case CConstants::eCommandLog:
        return log();

    case CConstants::eCommandDiff:
        return diff();

    case CConstants::eCommandMerge:
        return merge();

    case CConstants::eCommandDump:
        return dump();

    case CConstants::eCommandHelp:
        return help();
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------

bool AzeApp::isASainRepository()
{
    if (not m_pRepository->ok())
    {
        OUT_ERROR(Aze::CStrings::s_sTextNotARepository);

        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

void AzeApp::processWildCards()
{
    m_lFilesAndIds = CFileUtilities::getInstance()->concernedFiles(
                m_pRepository->database()->rootPath(),
                m_lFilesAndIds
                );
}

//-------------------------------------------------------------------------------------------------

int AzeApp::test()
{
    Aze::CTestAze test;
    QTEST_SET_MAIN_SOURCE_PATH
    return QTest::qExec(&test, 0, nullptr);
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

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::switchToBranch()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_lFilesAndIds.count() > 0, CConstants::s_iError_NoBranchNameGiven);

    ERROR_WHEN_FALSE(m_pRepository->switchToBranch(m_lFilesAndIds[0]), CConstants::s_iError_CouldNotSetCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->writeCurrentBranch(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->writeGeneralInfo(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::status()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    (*m_pOutStream) << "Status of working directory files, on branch " << m_pRepository->currentBranchName() << ":\n";

    QMap<Aze::CEnums::EFileStatus, bool> bVisibility;

    bVisibility[Aze::CEnums::eLoose] = getSwitch(CConstants::s_sSwitchLoose);
    bVisibility[Aze::CEnums::eClean] = getSwitch(CConstants::s_sSwitchClean);
    bVisibility[Aze::CEnums::eModified] = getSwitch(CConstants::s_sSwitchModified);
    bVisibility[Aze::CEnums::eAdded] = getSwitch(CConstants::s_sSwitchAdded);
    bVisibility[Aze::CEnums::eDeleted] = getSwitch(CConstants::s_sSwitchDeleted);
    bVisibility[Aze::CEnums::eMissing] = getSwitch(CConstants::s_sSwitchMissing);
    bVisibility[Aze::CEnums::eIgnored] = getSwitch(CConstants::s_sSwitchIgnored);
    bVisibility[Aze::CEnums::eAll] = getSwitch(CConstants::s_sSwitchAll);

    ERROR_WHEN_FALSE(checkRemainingSwitches(), CConstants::s_iError_UnknownSwitch);

    bool bAtLeastOneSwitchTrue = false;
    for (bool bSwitch : bVisibility.values())
        if (bSwitch)
            bAtLeastOneSwitchTrue = true;

    if (not bAtLeastOneSwitchTrue)
    {
        bVisibility[Aze::CEnums::eAdded] = true;
        bVisibility[Aze::CEnums::eDeleted] = true;
        bVisibility[Aze::CEnums::eModified] = true;
        bVisibility[Aze::CEnums::eMissing] = true;
    }

    QList<Aze::CFile> lFiles = m_pRepository->fileStatus(m_lFilesAndIds);

    for (Aze::CFile& file : lFiles)
    {
        if (bVisibility[Aze::CEnums::eAll] || bVisibility[file.status()])
        {
            (*m_pOutStream) << Aze::CEnums::FileStatusSymbol(file.status()) << " " << file.relativeName();
            (*m_pOutStream) << "\n";
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::stage()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_lFilesAndIds.count() > 0, CConstants::s_iError_NoFileNameGiven);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->stage(m_lFilesAndIds), CConstants::s_iError_CouldNotAddFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::unstage()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_lFilesAndIds.count() > 0, CConstants::s_iError_NoFileNameGiven);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->unstage(m_lFilesAndIds), CConstants::s_iError_CouldNotAddFiles);

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

int AzeApp::log()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    (*m_pOutStream) << m_pRepository->log(m_lFilesAndIds);
    m_pOutStream->flush();

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::diff()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    QString sFirst = m_lFilesAndIds.isEmpty() ? "" : m_lFilesAndIds.takeFirst();
    QString sSecond = m_lFilesAndIds.isEmpty() ? "" : m_lFilesAndIds.takeFirst();

    (*m_pOutStream) << m_pRepository->diff(sFirst, sSecond);
    m_pOutStream->flush();

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::merge()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_lFilesAndIds.count() > 0, CConstants::s_iError_NoBranchNameGiven);

    QString sBranchName = m_lFilesAndIds.takeFirst();

    if (sBranchName == m_pRepository->currentBranchName())
    {
        (*m_pOutStream) << "Sorry, you can't merge a branch on itself..." << "\n";
        m_pOutStream->flush();
        return CConstants::s_iError_CouldNotMerge;
    }

    ERROR_WHEN_FALSE(m_pRepository->merge(sBranchName), CConstants::s_iError_CouldNotMerge);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::dump()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    for (QString sId : m_lFilesAndIds)
    {
        (*m_pOutStream) << m_pRepository->database()->printableFileContentById(sId);
        m_pOutStream->flush();
    }

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int AzeApp::help()
{
    (*m_pOutStream) << CConstants::s_sTextCommands << ":\n\n";

    for (QString sKey : CConstants::s_mHelp.keys())
    {
        (*m_pOutStream) << QString(" %1  %2\n").arg(sKey, -15).arg(CConstants::s_mHelp[sKey]);
    }

    return CConstants::s_iError_None;
}
