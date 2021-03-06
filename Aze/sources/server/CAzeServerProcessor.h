
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

class CAzeServer;

class CAzeServerProcessor : public CDynamicHTTPServer
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructor & destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CAzeServerProcessor(CAzeServer* parent, quint16 iPort = DEFAULT_HTTP_PORT);

    //! Destructor
    virtual ~CAzeServerProcessor() override;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void getContent(CWebContext& tContext, QString& sHead, QString& sBody, QString& sCustomResponse, QString& sCustomResponseMIME) override;

protected:

    //-------------------------------------------------------------------------------------------------
    // Protected control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CXMLNode serveRequest(CWebContext& tContext, const CXMLNode& xRequest);

    //!
    static CXMLNode servePull(CWebContext& tContext, const CXMLNode& xRequest);

    //!
    static CXMLNode servePush(CWebContext& tContext, const CXMLNode& xRequest);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    typedef CXMLNode (*RequestServing) (CWebContext& tContext, const CXMLNode& xRequest);

    CAzeServer*                     m_pServer;
    QMap<QString, RequestServing>   m_mRequestFunctionMap;
};

//-------------------------------------------------------------------------------------------------
