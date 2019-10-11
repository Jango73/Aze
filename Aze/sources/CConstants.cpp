
// Qt
#include <QDebug>

// Application
#include "CConstants.h"

//-------------------------------------------------------------------------------------------------

const char* CConstants::s_sSwitchInitRepository     = "init";
const char* CConstants::s_sSwitchShowStatus         = "status";
const char* CConstants::s_sSwitchAdd                = "add";
const char* CConstants::s_sSwitchMove               = "move";
const char* CConstants::s_sSwitchRemove             = "remove";
const char* CConstants::s_sSwitchCommit             = "commit";
const char* CConstants::s_sSwitchBranch             = "branch";

const int CConstants::s_iError_None                 = 0;
const int CConstants::s_iError_NotARepository       = 1;
const int CConstants::s_iError_CouldNotReadStage    = 2;
const int CConstants::s_iError_CouldNotWriteStage   = 3;
const int CConstants::s_iError_CouldNotAddFiles     = 4;
const int CConstants::s_iError_CouldNotMoveFiles    = 5;
const int CConstants::s_iError_CouldNotRemoveFiles  = 6;
