
// Qt
#include <QDebug>

// Application
#include "CConstants.h"

//-------------------------------------------------------------------------------------------------

const char* CConstants::s_sContextMain                      = "main";

const char* CConstants::s_sSwitchHelpOn                     = "help";
const char* CConstants::s_sSwitchInitRepository             = "init";
const char* CConstants::s_sSwitchCreateBranch               = "create-branch";
const char* CConstants::s_sSwitchSwitchToBranch             = "set-branch";
const char* CConstants::s_sSwitchShowStatus                 = "status";
const char* CConstants::s_sSwitchStage                      = "stage";
const char* CConstants::s_sSwitchUnstage                    = "unstage";
const char* CConstants::s_sSwitchRevert                     = "revert";
const char* CConstants::s_sSwitchMove                       = "move";
const char* CConstants::s_sSwitchRemove                     = "remove";
const char* CConstants::s_sSwitchCommit                     = "commit";
const char* CConstants::s_sSwitchCleanUp                    = "clean-up";
const char* CConstants::s_sSwitchLog                        = "log";
const char* CConstants::s_sSwitchDiff                       = "diff";
const char* CConstants::s_sSwitchMerge                      = "merge";
const char* CConstants::s_sSwitchDump                       = "show";

const char* CConstants::s_sSwitchAuthor                     = "author";
const char* CConstants::s_sSwitchMessage                    = "message";
const char* CConstants::s_sSwitchAll                        = "all";
const char* CConstants::s_sSwitchLoose                      = "loose";
const char* CConstants::s_sSwitchClean                      = "clean";
const char* CConstants::s_sSwitchModified                   = "modified";
const char* CConstants::s_sSwitchAdded                      = "added";
const char* CConstants::s_sSwitchDeleted                    = "deleted";
const char* CConstants::s_sSwitchMissing                    = "missing";
const char* CConstants::s_sSwitchIgnored                    = "ignored";
const char* CConstants::s_sSwitchStart                      = "start";
const char* CConstants::s_sSwitchCount                      = "count";
const char* CConstants::s_sSwitchAllowFileDelete            = "allow-delete";
const char* CConstants::s_sSwitchGraph                      = "graph";

const int CConstants::s_iError_None                         = 0;
const int CConstants::s_iError_UnknownCommand               = 1;
const int CConstants::s_iError_UnknownSwitch                = 2;
const int CConstants::s_iError_NotARepository               = 3;
const int CConstants::s_iError_NoBranchNameGiven            = 4;
const int CConstants::s_iError_NoFileNameGiven              = 5;
const int CConstants::s_iError_CouldNotCreateBranch         = 6;
const int CConstants::s_iError_CouldNotSetCurrentBranch     = 7;
const int CConstants::s_iError_CouldNotReadStage            = 8;
const int CConstants::s_iError_CouldNotWriteStage           = 9;
const int CConstants::s_iError_CouldNotWriteCurrentBranch   = 10;
const int CConstants::s_iError_CouldNotAddFiles             = 11;
const int CConstants::s_iError_CouldNotRevertFiles          = 12;
const int CConstants::s_iError_CouldNotMoveFiles            = 13;
const int CConstants::s_iError_CouldNotRemoveFiles          = 14;
const int CConstants::s_iError_CouldNotMerge                = 15;

const QString CConstants::s_sAllFilesAreClean               = tr("All files are clean on branch %1.");
const QString CConstants::s_sStatusOfFiles                  = tr("Status of working directory files, on branch %1:");
const QString CConstants::s_sTextCommands                   = tr("Commands");
const QString CConstants::s_sTextYouAreNowOnBranch          = tr("You are now on branch %1.");
const QString CConstants::s_sTextYouAreAlreadyOnBranch      = tr("You are already on branch %1.");

QMap<QString, CConstants::ECommand> CConstants::s_mCommands;
QMap<QString, QString> CConstants::s_mHelp;

//-------------------------------------------------------------------------------------------------

void CConstants::initCommandMap()
{
    s_mCommands[CConstants::s_sSwitchInitRepository]    = CConstants::eCommandInitRepository;
    s_mCommands[CConstants::s_sSwitchCreateBranch]      = CConstants::eCommandCreateBranch;
    s_mCommands[CConstants::s_sSwitchSwitchToBranch]    = CConstants::eCommandSwitchToBranch;
    s_mCommands[CConstants::s_sSwitchShowStatus]        = CConstants::eCommandShowStatus;
    s_mCommands[CConstants::s_sSwitchStage]             = CConstants::eCommandStage;
    s_mCommands[CConstants::s_sSwitchUnstage]           = CConstants::eCommandUnstage;
    s_mCommands[CConstants::s_sSwitchRevert]            = CConstants::eCommandRevert;
    s_mCommands[CConstants::s_sSwitchMove]              = CConstants::eCommandMove;
    s_mCommands[CConstants::s_sSwitchRemove]            = CConstants::eCommandRemove;
    s_mCommands[CConstants::s_sSwitchCommit]            = CConstants::eCommandCommit;
    s_mCommands[CConstants::s_sSwitchCleanUp]           = CConstants::eCommandCleanUp;
    s_mCommands[CConstants::s_sSwitchLog]               = CConstants::eCommandLog;
    s_mCommands[CConstants::s_sSwitchDiff]              = CConstants::eCommandDiff;
    s_mCommands[CConstants::s_sSwitchMerge]             = CConstants::eCommandMerge;
    s_mCommands[CConstants::s_sSwitchDump]              = CConstants::eCommandDump;

    s_mHelp[CConstants::s_sSwitchInitRepository]        = tr("Makes the current directory an Aze repository.");
    s_mHelp[CConstants::s_sSwitchCreateBranch]          = tr("Creates a branch.");
    s_mHelp[CConstants::s_sSwitchSwitchToBranch]        = tr("Switches to a branch.");
    s_mHelp[CConstants::s_sSwitchShowStatus]            = tr("Shows status of files.");
    s_mHelp[CConstants::s_sSwitchStage]                 = tr("Adds some files to the staged.");
    s_mHelp[CConstants::s_sSwitchUnstage]               = tr("Removes some files from the stage.");
    s_mHelp[CConstants::s_sSwitchRevert]                = tr("Reverts some files using tip commit.");
    s_mHelp[CConstants::s_sSwitchMove]                  = tr("Moves some files.");
    s_mHelp[CConstants::s_sSwitchRemove]                = tr("Removes some files from tracking.");
    s_mHelp[CConstants::s_sSwitchCommit]                = tr("Creates a commit from the stage.");
    s_mHelp[CConstants::s_sSwitchCleanUp]               = tr("Clears the stage and reverts all files. Warning: loose files are deleted.");
    s_mHelp[CConstants::s_sSwitchLog]                   = tr("Shows a log of a branch.");
    s_mHelp[CConstants::s_sSwitchDiff]                  = tr("Shows a diff between two commits or files.");
    s_mHelp[CConstants::s_sSwitchDump]                  = tr("Dumps the content of a database object.");
}
