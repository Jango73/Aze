
// Qt
#include <QDebug>

// Application
#include "CConstants.h"

//-------------------------------------------------------------------------------------------------

const char* CConstants::s_sSwitchInitRepository             = "init";
const char* CConstants::s_sSwitchCreateBranch               = "create-branch";
const char* CConstants::s_sSwitchShowStatus                 = "status";
const char* CConstants::s_sSwitchAdd                        = "add";
const char* CConstants::s_sSwitchMove                       = "move";
const char* CConstants::s_sSwitchRemove                     = "remove";
const char* CConstants::s_sSwitchCommit                     = "commit";
const char* CConstants::s_sSwitchDiff                       = "diff";
const char* CConstants::s_sSwitchDump                       = "show";

const char* CConstants::s_sSwitchAuthor                     = "--author";
const char* CConstants::s_sSwitchMessage                    = "--message";

const int CConstants::s_iError_None                         = 0;
const int CConstants::s_iError_NotARepository               = 1;
const int CConstants::s_iError_NoBranchNameGiven            = 2;
const int CConstants::s_iError_CouldNotCreateBranch         = 3;
const int CConstants::s_iError_CouldNotReadStage            = 4;
const int CConstants::s_iError_CouldNotWriteStage           = 5;
const int CConstants::s_iError_CouldNotWriteCurrentBranch   = 6;
const int CConstants::s_iError_CouldNotAddFiles             = 7;
const int CConstants::s_iError_CouldNotMoveFiles            = 8;
const int CConstants::s_iError_CouldNotRemoveFiles          = 9;

QMap<QString, CConstants::ECommand> CConstants::s_mCommands;

//-------------------------------------------------------------------------------------------------

void CConstants::initCommandMap()
{
    s_mCommands[CConstants::s_sSwitchInitRepository]    = CConstants::eCommandInitRepository;
    s_mCommands[CConstants::s_sSwitchCreateBranch]      = CConstants::eCommandCreateBranch;
    s_mCommands[CConstants::s_sSwitchShowStatus]        = CConstants::eCommandShowStatus;
    s_mCommands[CConstants::s_sSwitchAdd]               = CConstants::eCommandAdd;
    s_mCommands[CConstants::s_sSwitchMove]              = CConstants::eCommandMove;
    s_mCommands[CConstants::s_sSwitchRemove]            = CConstants::eCommandRemove;
    s_mCommands[CConstants::s_sSwitchCommit]            = CConstants::eCommandCommit;
    s_mCommands[CConstants::s_sSwitchDump]              = CConstants::eCommandDump;
}
