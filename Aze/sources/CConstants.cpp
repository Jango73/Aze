
// Qt
#include <QDebug>

// Application
#include "CConstants.h"

//-------------------------------------------------------------------------------------------------

const char* CConstants::s_sSwitchRunTests                   = "run-tests";
const char* CConstants::s_sSwitchInitRepository             = "init";
const char* CConstants::s_sSwitchCreateBranch               = "create-branch";
const char* CConstants::s_sSwitchSwitchToBranch             = "set-branch";
const char* CConstants::s_sSwitchShowStatus                 = "status";
const char* CConstants::s_sSwitchStage                      = "stage";
const char* CConstants::s_sSwitchUnstage                    = "unstage";
const char* CConstants::s_sSwitchMove                       = "move";
const char* CConstants::s_sSwitchRemove                     = "remove";
const char* CConstants::s_sSwitchCommit                     = "commit";
const char* CConstants::s_sSwitchLog                        = "log";
const char* CConstants::s_sSwitchDiff                       = "diff";
const char* CConstants::s_sSwitchMerge                      = "merge";
const char* CConstants::s_sSwitchDump                       = "show";
const char* CConstants::s_sSwitchHelp                       = "help";

const char* CConstants::s_sSwitchAuthor                     = "--author";
const char* CConstants::s_sSwitchMessage                    = "--message";
const char* CConstants::s_sSwitchAll                        = "--all";
const char* CConstants::s_sSwitchLoose                      = "--loose";
const char* CConstants::s_sSwitchClean                      = "--clean";
const char* CConstants::s_sSwitchModified                   = "--modified";
const char* CConstants::s_sSwitchAdded                      = "--added";
const char* CConstants::s_sSwitchDeleted                    = "--deleted";
const char* CConstants::s_sSwitchMissing                    = "--missing";
const char* CConstants::s_sSwitchIgnored                    = "--ignored";

const int CConstants::s_iError_None                         = 0;
const int CConstants::s_iError_UnknownSwitch                = 1;
const int CConstants::s_iError_NotARepository               = 2;
const int CConstants::s_iError_NoBranchNameGiven            = 3;
const int CConstants::s_iError_NoFileNameGiven              = 4;
const int CConstants::s_iError_CouldNotCreateBranch         = 5;
const int CConstants::s_iError_CouldNotSetCurrentBranch     = 6;
const int CConstants::s_iError_CouldNotReadStage            = 7;
const int CConstants::s_iError_CouldNotWriteStage           = 8;
const int CConstants::s_iError_CouldNotWriteCurrentBranch   = 9;
const int CConstants::s_iError_CouldNotAddFiles             = 10;
const int CConstants::s_iError_CouldNotMoveFiles            = 11;
const int CConstants::s_iError_CouldNotRemoveFiles          = 12;
const int CConstants::s_iError_CouldNotMerge                = 13;

const QString CConstants::s_sTextCommands                   = tr("Commands");

QMap<QString, CConstants::ECommand> CConstants::s_mCommands;
QMap<QString, QString> CConstants::s_mHelp;

//-------------------------------------------------------------------------------------------------

void CConstants::initCommandMap()
{
    s_mCommands[CConstants::s_sSwitchRunTests]          = CConstants::eCommandRunTests;
    s_mCommands[CConstants::s_sSwitchInitRepository]    = CConstants::eCommandInitRepository;
    s_mCommands[CConstants::s_sSwitchCreateBranch]      = CConstants::eCommandCreateBranch;
    s_mCommands[CConstants::s_sSwitchSwitchToBranch]    = CConstants::eCommandSwitchToBranch;
    s_mCommands[CConstants::s_sSwitchShowStatus]        = CConstants::eCommandShowStatus;
    s_mCommands[CConstants::s_sSwitchStage]             = CConstants::eCommandStage;
    s_mCommands[CConstants::s_sSwitchUnstage]           = CConstants::eCommandUnstage;
    s_mCommands[CConstants::s_sSwitchMove]              = CConstants::eCommandMove;
    s_mCommands[CConstants::s_sSwitchRemove]            = CConstants::eCommandRemove;
    s_mCommands[CConstants::s_sSwitchCommit]            = CConstants::eCommandCommit;
    s_mCommands[CConstants::s_sSwitchLog]               = CConstants::eCommandLog;
    s_mCommands[CConstants::s_sSwitchDiff]              = CConstants::eCommandDiff;
    s_mCommands[CConstants::s_sSwitchMerge]             = CConstants::eCommandMerge;
    s_mCommands[CConstants::s_sSwitchDump]              = CConstants::eCommandDump;
    s_mCommands[CConstants::s_sSwitchHelp]              = CConstants::eCommandHelp;

    s_mHelp[CConstants::s_sSwitchRunTests]              = tr("Runs Aze unit tests. Please do this in an empty repository.");
    s_mHelp[CConstants::s_sSwitchInitRepository]        = tr("Makes the current directory an Aze repository.");
    s_mHelp[CConstants::s_sSwitchCreateBranch]          = tr("Creates a branch.");
    s_mHelp[CConstants::s_sSwitchSwitchToBranch]        = tr("Switches to a branch.");
    s_mHelp[CConstants::s_sSwitchShowStatus]            = tr("Shows status of files.");
    s_mHelp[CConstants::s_sSwitchStage]                 = tr("Adds some files to the staged.");
    s_mHelp[CConstants::s_sSwitchUnstage]               = tr("Removes some files from the stage.");
    s_mHelp[CConstants::s_sSwitchMove]                  = tr("Moves some files.");
    s_mHelp[CConstants::s_sSwitchRemove]                = tr("Removes some files from tracking.");
    s_mHelp[CConstants::s_sSwitchCommit]                = tr("Creates a commit from the stage.");
    s_mHelp[CConstants::s_sSwitchLog]                   = tr("Shows a log of a branch.");
    s_mHelp[CConstants::s_sSwitchDiff]                  = tr("Shows a diff between two commits or files.");
    s_mHelp[CConstants::s_sSwitchDump]                  = tr("Dumps the content of a database object.");
}
