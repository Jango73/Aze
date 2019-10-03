
#pragma once

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

class CConstants : public QObject
{
    Q_OBJECT

public:

    // Branch types
    enum ECommand
    {
        eCommandNone,
        eCommandShowStatus
    };

    // Switches
    static const char* s_sSwitchShowStatus;
};
