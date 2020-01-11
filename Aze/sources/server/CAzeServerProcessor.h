
#pragma once

// Qt
#include <QObject>

// qt-plus
#include "Web/CDynamicHTTPServer.h"

// AzeLib
#include "CRepository.h"

// Application
#include "../common/CConstants.h"

//-------------------------------------------------------------------------------------------------

class CAzeServerProcessor : public CDynamicHTTPServer
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CAzeServerProcessor(quint16 iPort = DEFAULT_HTTP_PORT);

    //! Destructor
    virtual ~CAzeServerProcessor() override;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void getContent(CWebContext& tContext, QString& sHead, QString& sBody, QString& sCustomResponse, QString& sCustomResponseMIME) override;

    //!
    CXMLNode serveRequest(CXMLNode xRequest);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

};

//-------------------------------------------------------------------------------------------------
