
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

#define ERROR_WHEN_FALSE(a,b)    if ((a) == false) return b;

//-------------------------------------------------------------------------------------------------

class CConstants : public QObject
{
    Q_OBJECT

public:

    // Commands
    enum ECommand
    {
        eCommandNone,
        eCommandTest,
        eCommandInitRepository,
        eCommandCreateBranch,
        eCommandShowStatus,
        eCommandStage,
        eCommandUnstage,
        eCommandMove,
        eCommandRemove,
        eCommandCommit,
        eCommandDiff,
        eCommandDump
    };

    // Switches
    static const char* s_sSwitchTest;
    static const char* s_sSwitchInitRepository;
    static const char* s_sSwitchCreateBranch;
    static const char* s_sSwitchShowStatus;
    static const char* s_sSwitchStage;
    static const char* s_sSwitchUnstage;
    static const char* s_sSwitchMove;
    static const char* s_sSwitchRemove;
    static const char* s_sSwitchCommit;
    static const char* s_sSwitchDiff;
    static const char* s_sSwitchDump;

    static const char* s_sSwitchAuthor;
    static const char* s_sSwitchMessage;

    static const int s_iError_None;
    static const int s_iError_NotARepository;
    static const int s_iError_NoBranchNameGiven;
    static const int s_iError_NoFileNameGiven;
    static const int s_iError_CouldNotCreateBranch;
    static const int s_iError_CouldNotReadStage;
    static const int s_iError_CouldNotWriteStage;
    static const int s_iError_CouldNotWriteCurrentBranch;
    static const int s_iError_CouldNotAddFiles;
    static const int s_iError_CouldNotMoveFiles;
    static const int s_iError_CouldNotRemoveFiles;

    static QMap<QString, CConstants::ECommand> s_mCommands;
    static QMap<QString, QString> s_mHelp;

    //!
    static void initCommandMap();
};
