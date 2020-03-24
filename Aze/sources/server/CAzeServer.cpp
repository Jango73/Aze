
// Qt
#include <QDebug>
#include <QDir>
#include <QDirIterator>

// qt-plus
#include <CXMLNode.h>
#include <File/CFileUtilities.h>

// Aze
#include "CUtils.h"

// Application
#include "CAzeServer.h"

/*!
    \class CAzeServer
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

CAzeServer::CAzeServer(int argc, char *argv[])
    : QCoreApplication(argc, argv)
    , m_sRepositoriesBasePath(".")
    , m_tArguments(*this)
    , m_pProcessor(new CAzeServerProcessor(this, m_tArguments.m_tParser.value(m_tArguments.m_oPort).toInt()))
{
}

//-------------------------------------------------------------------------------------------------

CAzeServer::~CAzeServer()
{
}

//-------------------------------------------------------------------------------------------------
