
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
    , m_tArguments(*this)
{
}

//-------------------------------------------------------------------------------------------------

CAzeServer::~CAzeServer()
{
}

//-------------------------------------------------------------------------------------------------
