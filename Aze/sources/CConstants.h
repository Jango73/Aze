
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

#define ERROR_WHEN_FALSE(a,b)    if (a == false) return b;

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
        eCommandShowStatus,
        eCommandAdd,
        eCommandMove,
        eCommandRemove,
        eCommandCommit,
        eCommandDump
    };

    // Switches
    static const char* s_sSwitchInitRepository;
    static const char* s_sSwitchShowStatus;
    static const char* s_sSwitchAdd;
    static const char* s_sSwitchMove;
    static const char* s_sSwitchRemove;
    static const char* s_sSwitchCommit;
    static const char* s_sSwitchBranch;
    static const char* s_sSwitchDump;

    static const char* s_sSwitchMessage;

    static const int s_iError_None;
    static const int s_iError_NotARepository;
    static const int s_iError_CouldNotReadStage;
    static const int s_iError_CouldNotWriteStage;
    static const int s_iError_CouldNotWriteCurrentBranch;
    static const int s_iError_CouldNotAddFiles;
    static const int s_iError_CouldNotMoveFiles;
    static const int s_iError_CouldNotRemoveFiles;
};
