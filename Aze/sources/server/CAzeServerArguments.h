
#pragma once

// Qt
#include <QObject>
#include <QCommandLineParser>
#include <QCommandLineOption>

// Application
#include "../common/CConstants.h"

//-------------------------------------------------------------------------------------------------

class CAzeServerArguments : public QObject
{
public:

    CAzeServerArguments(QCoreApplication& app);

    QCommandLineParser      m_tParser;
    QCommandLineOption      m_oPort;
};

//-------------------------------------------------------------------------------------------------
