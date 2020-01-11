
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

#define ERROR_WHEN_FALSE(a,b)           if ((a) == false) return b

#define ERROR_WHEN_FALSE_PRINT(a,b,c)   if ((a) == false) { OUT_ERROR(b); return c; }

#define PROPNAME_ACTION                 "action"

//-------------------------------------------------------------------------------------------------

class CConstants : public QObject
{
    Q_OBJECT

public:

    // Commands
    enum ECommand
    {
        eCommandNone,
        eCommandInitRepository,
        eCommandCreateBranch,
        eCommandSwitchToBranch,
        eCommandShowStatus,
        eCommandStage,
        eCommandUnstage,
        eCommandRevert,
        eCommandMove,
        eCommandRemove,
        eCommandCommit,
        eCommandCleanUp,
        eCommandLog,
        eCommandDiff,
        eCommandMerge,
        eCommandDump
    };

    static const char* s_sContextMain;

    // Commands
    static const char* s_sSwitchHelpOn;
    static const char* s_sSwitchInitRepository;
    static const char* s_sSwitchCreateBranch;
    static const char* s_sSwitchSwitchToBranch;
    static const char* s_sSwitchShowStatus;
    static const char* s_sSwitchStage;
    static const char* s_sSwitchUnstage;
    static const char* s_sSwitchRevert;
    static const char* s_sSwitchMove;
    static const char* s_sSwitchRemove;
    static const char* s_sSwitchCommit;
    static const char* s_sSwitchCleanUp;
    static const char* s_sSwitchLog;
    static const char* s_sSwitchDiff;
    static const char* s_sSwitchMerge;
    static const char* s_sSwitchDump;

    // Client switches
    static const char* s_sSwitchAuthor;
    static const char* s_sSwitchMessage;
    static const char* s_sSwitchAll;
    static const char* s_sSwitchLoose;
    static const char* s_sSwitchClean;
    static const char* s_sSwitchModified;
    static const char* s_sSwitchAdded;
    static const char* s_sSwitchDeleted;
    static const char* s_sSwitchMissing;
    static const char* s_sSwitchIgnored;
    static const char* s_sSwitchStart;
    static const char* s_sSwitchCount;
    static const char* s_sSwitchAllowFileDelete;
    static const char* s_sSwitchGraph;

    // Server switches
    static const char* s_sSwitchPort;

    // Error codes
    static const int s_iError_None;
    static const int s_iError_UnknownCommand;
    static const int s_iError_UnknownSwitch;
    static const int s_iError_NotARepository;
    static const int s_iError_NoBranchNameGiven;
    static const int s_iError_NoFileNameGiven;
    static const int s_iError_CouldNotCreateBranch;
    static const int s_iError_CouldNotSetCurrentBranch;
    static const int s_iError_CouldNotReadStage;
    static const int s_iError_CouldNotWriteStage;
    static const int s_iError_CouldNotWriteCurrentBranch;
    static const int s_iError_CouldNotAddFiles;
    static const int s_iError_CouldNotRevertFiles;
    static const int s_iError_CouldNotMoveFiles;
    static const int s_iError_CouldNotRemoveFiles;
    static const int s_iError_CouldNotMerge;

    // Text strings
    static const QString s_sAllFilesAreClean;
    static const QString s_sStatusOfFiles;
    static const QString s_sTextCommands;
    static const QString s_sTextYouAreNowOnBranch;
    static const QString s_sTextYouAreAlreadyOnBranch;

    static QMap<QString, CConstants::ECommand> s_mCommands;
    static QMap<QString, QString> s_mHelp;

    //!
    static void initCommandMap();
};