
// Qt
#include <QDebug>
#include <QDir>
#include <QDirIterator>

// qt-plus
#include <CXMLNode.h>
#include <File/CFileUtilities.h>

// Aze
#include "CUtils.h"

// Application
#include "CAzeClient.h"

/*!
    \class CAzeClient
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

CAzeClient::CAzeClient(int argc, char *argv[], QTextStream* pOutStream)
    : QCoreApplication(argc, argv)
    , m_tArguments(*this)
    , m_pOutStream(pOutStream)
    , m_pRepository(new Aze::CRepository(QDir::currentPath(), this))
    , m_bOutputStreamIsMine(false)
{
    if (m_pOutStream == nullptr)
    {
        m_bOutputStreamIsMine = true;
        m_pOutStream = new QTextStream(stdout);
    }
}

//-------------------------------------------------------------------------------------------------

CAzeClient::~CAzeClient()
{
    if (m_bOutputStreamIsMine)
        delete m_pOutStream;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::run()
{
    switch (m_tArguments.m_eCommand)
    {
    case CConstants::eCommandNone:
        break;

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

    case CConstants::eCommandRevert:
        return revert();

    case CConstants::eCommandMove:
        return move();

    case CConstants::eCommandRemove:
        return remove();

    case CConstants::eCommandCommit:
        return commit();

    case CConstants::eCommandCleanUp:
        return cleanUp();

    case CConstants::eCommandLog:
        return log();

    case CConstants::eCommandDiff:
        return diff();

    case CConstants::eCommandMerge:
        return merge();

    case CConstants::eCommandDump:
        return dump();
    }

    OUT_ERROR("Unknown command...");

    return 0;
}

//-------------------------------------------------------------------------------------------------

bool CAzeClient::isASainRepository()
{
    if (not m_pRepository->ok())
    {
        OUT_ERROR(Aze::CStrings::s_sTextNotARepository);

        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

void CAzeClient::processWildCards()
{
    m_tArguments.m_lFilesAndIds = CFileUtilities::getInstance()->concernedFiles(
                m_pRepository->database()->rootPath(),
                m_tArguments.m_lFilesAndIds
                );
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::init()
{
    return m_pRepository->init() ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::createBranch()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_tArguments.m_lFilesAndIds.count() > 0, CConstants::s_iError_NoBranchNameGiven);

    ERROR_WHEN_FALSE(m_pRepository->createBranch(m_tArguments.m_lFilesAndIds[0]), CConstants::s_iError_CouldNotCreateBranch);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::switchToBranch()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_tArguments.m_lFilesAndIds.count() > 0, CConstants::s_iError_NoBranchNameGiven);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    bool bAllowFileDelete = m_tArguments.m_tParser.isSet(m_tArguments.m_oAllowFileDelete);

    QString sBranchName = m_tArguments.m_lFilesAndIds[0];

    ERROR_WHEN_FALSE_PRINT(
                sBranchName != m_pRepository->currentBranchName(),
                QString(CConstants::s_sTextYouAreAlreadyOnBranch).arg(sBranchName),
                CConstants::s_iError_None
                );

    ERROR_WHEN_FALSE(m_pRepository->switchToBranch(sBranchName, bAllowFileDelete), CConstants::s_iError_CouldNotSetCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->writeCurrentBranch(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->writeGeneralInfo(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    (*m_pOutStream) << QString(CConstants::s_sTextYouAreNowOnBranch).arg(sBranchName) << "\n";

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::status()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    QMap<Aze::CEnums::EFileStatus, bool> bVisibility;

    bVisibility[Aze::CEnums::eLoose] = m_tArguments.m_tParser.isSet(m_tArguments.m_oLoose);
    bVisibility[Aze::CEnums::eClean] = m_tArguments.m_tParser.isSet(m_tArguments.m_oClean);
    bVisibility[Aze::CEnums::eModified] = m_tArguments.m_tParser.isSet(m_tArguments.m_oModified);
    bVisibility[Aze::CEnums::eAdded] = m_tArguments.m_tParser.isSet(m_tArguments.m_oAdded);
    bVisibility[Aze::CEnums::eDeleted] = m_tArguments.m_tParser.isSet(m_tArguments.m_oDeleted);
    bVisibility[Aze::CEnums::eMissing] = m_tArguments.m_tParser.isSet(m_tArguments.m_oMissing);
    bVisibility[Aze::CEnums::eIgnored] = m_tArguments.m_tParser.isSet(m_tArguments.m_oIgnored);
    bVisibility[Aze::CEnums::eAll] = m_tArguments.m_tParser.isSet(m_tArguments.m_oAll);

    processWildCards();

    QString sOutput;
    QList<Aze::CFile> lFiles = m_pRepository->fileStatus(m_tArguments.m_lFilesAndIds);

    if (not m_tArguments.m_lFilesAndIds.isEmpty())
    {
        for (QString sFullName : m_tArguments.m_lFilesAndIds)
        {
            QString sRelativeName = m_pRepository->database()->relativeFileName(sFullName);

            QList<Aze::CFile>::iterator file = std::find_if(
                        lFiles.begin(),
                        lFiles.end(),
                        [&](const Aze::CFile& f) { return f.relativeName() == sRelativeName; }
            );

            if (file != lFiles.end())
            {
                sOutput += QString("%1 %2\n")
                        .arg(Aze::CEnums::FileStatusSymbol(file->status()))
                        .arg(sRelativeName);
            }
        }
    }
    else
    {
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

        for (Aze::CFile& file : lFiles)
        {
            QString sRelativeName = file.relativeName();

            if (bVisibility[Aze::CEnums::eAll] || bVisibility[file.status()])
            {
                sOutput += QString("%1 %2\n")
                        .arg(Aze::CEnums::FileStatusSymbol(file.status()))
                        .arg(sRelativeName);
            }
        }
    }

    if (sOutput.isEmpty())
    {
        sOutput = QString(CConstants::s_sAllFilesAreClean)
                .arg(m_pRepository->currentBranchName())
                + QString("\n");
    }
    else
    {
        sOutput = QString(CConstants::s_sStatusOfFiles)
                .arg(m_pRepository->currentBranchName())
                + QString("\n")
                + sOutput;
    }

    (*m_pOutStream) << sOutput;

    return 0;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::stage()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE_PRINT(
                m_tArguments.m_lFilesAndIds.count() > 0,
                Aze::CStrings::s_sTextNoFileSpecified,
                CConstants::s_iError_NoFileNameGiven
                );

    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->stage(m_tArguments.m_lFilesAndIds), CConstants::s_iError_CouldNotAddFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::unstage()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE_PRINT(
                m_tArguments.m_lFilesAndIds.count() > 0,
                Aze::CStrings::s_sTextNoFileSpecified,
                CConstants::s_iError_NoFileNameGiven
                );

    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->unstage(m_tArguments.m_lFilesAndIds), CConstants::s_iError_CouldNotAddFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::revert()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    bool bAllowFileDelete = m_tArguments.m_tParser.isSet(m_tArguments.m_oAllowFileDelete);

    ERROR_WHEN_FALSE(m_tArguments.m_lFilesAndIds.count() > 0, CConstants::s_iError_NoFileNameGiven);

    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->revert(m_tArguments.m_lFilesAndIds, bAllowFileDelete), CConstants::s_iError_CouldNotRevertFiles);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::move()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::remove()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_tArguments.m_lFilesAndIds.count() > 0, CConstants::s_iError_NoFileNameGiven);

    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->remove(m_tArguments.m_lFilesAndIds), CConstants::s_iError_CouldNotRemoveFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::commit()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    QString sAuthor = m_tArguments.m_tParser.value(m_tArguments.m_oAuthor);
    QString sMessage = m_tArguments.m_tParser.value(m_tArguments.m_oMessage);

    ERROR_WHEN_FALSE(m_pRepository->commit(sAuthor, sMessage), CConstants::s_iError_CouldNotRemoveFiles);

    ERROR_WHEN_FALSE(m_pRepository->writeCurrentBranch(), CConstants::s_iError_CouldNotWriteCurrentBranch);

    ERROR_WHEN_FALSE(m_pRepository->clearStage(), CConstants::s_iError_CouldNotWriteStage);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::cleanUp()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);
    ERROR_WHEN_FALSE(m_pRepository->clearStage(), CConstants::s_iError_CouldNotWriteStage);
    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    m_tArguments.m_lFilesAndIds.clear();
    m_tArguments.m_lFilesAndIds << "*";
    processWildCards();

    ERROR_WHEN_FALSE(m_pRepository->revert(m_tArguments.m_lFilesAndIds, true), CConstants::s_iError_CouldNotRevertFiles);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::log()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    bool bGraph = m_tArguments.m_tParser.isSet(m_tArguments.m_oGraph);
    int iStart = m_tArguments.m_tParser.value(m_tArguments.m_oStart).toInt();
    int iCount = m_tArguments.m_tParser.value(m_tArguments.m_oCount).toInt();

    (*m_pOutStream) << m_pRepository->log(m_tArguments.m_lFilesAndIds, bGraph, iStart, iCount);
    m_pOutStream->flush();

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::diff()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    QString sFirst = m_tArguments.m_lFilesAndIds.isEmpty() ? "" : m_tArguments.m_lFilesAndIds.takeFirst();
    QString sSecond = m_tArguments.m_lFilesAndIds.isEmpty() ? "" : m_tArguments.m_lFilesAndIds.takeFirst();

    (*m_pOutStream) << Aze::CUtils::printableUnifiedDiff(m_pRepository->diff(sFirst, sSecond));
    m_pOutStream->flush();

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::merge()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    ERROR_WHEN_FALSE(m_pRepository->readStage(), CConstants::s_iError_CouldNotReadStage);

    ERROR_WHEN_FALSE(m_tArguments.m_lFilesAndIds.count() > 0, CConstants::s_iError_NoBranchNameGiven);

    QString sBranchName = m_tArguments.m_lFilesAndIds.takeFirst();

    if (sBranchName == m_pRepository->currentBranchName())
    {
        (*m_pOutStream) << "Sorry, you can't merge a branch on itself..." << "\n";
        m_pOutStream->flush();
        return CConstants::s_iError_CouldNotMerge;
    }

    ERROR_WHEN_FALSE(m_pRepository->merge(sBranchName), CConstants::s_iError_CouldNotMerge);

    ERROR_WHEN_FALSE(m_pRepository->writeStage(), CConstants::s_iError_CouldNotWriteStage);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::dump()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    for (QString sId : m_tArguments.m_lFilesAndIds)
    {
        (*m_pOutStream) << m_pRepository->database()->printableFileContentById(sId);
        m_pOutStream->flush();
    }

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::help()
{
    (*m_pOutStream) << CConstants::s_sTextCommands << ":\n\n";

    for (QString sKey : CConstants::s_mHelp.keys())
    {
        (*m_pOutStream) << QString(" %1  %2\n").arg(sKey, -15).arg(CConstants::s_mHelp[sKey]);
    }

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------
