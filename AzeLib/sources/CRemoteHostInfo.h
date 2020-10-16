
#pragma once

// Qt
#include <QObject>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

// qt-plus
#include "Macros.h"

// Application
#include "CStrings.h"

//-------------------------------------------------------------------------------------------------

class CRemoteHostInfo : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

    Q_FAST_PROPERTY(QString, s, name, Name)
    Q_FAST_PROPERTY(QString, s, repositoryPath, RepositoryPath)

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

public:

    CRemoteHostInfo(QObject* parent = nullptr);
};

//-------------------------------------------------------------------------------------------------
