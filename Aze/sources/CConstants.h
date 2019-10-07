
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

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
        eCommandCommit
    };

    // Switches
    static const char* s_sSwitchInitRepository;
    static const char* s_sSwitchStatus;
    static const char* s_sSwitchAdd;
    static const char* s_sSwitchCommit;
    static const char* s_sSwitchBranch;
};
