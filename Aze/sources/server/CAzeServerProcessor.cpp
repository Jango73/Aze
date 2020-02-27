
// Qt
#include <QDebug>

// Aze
#include "CUtils.h"

// Application
#include "CAzeServerProcessor.h"

/*!
    \class CAzeServer
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

/*
    AZE Request format

    {
        "repository": "<Name of repository>"
        "branch": "<Name of branch>"
        "request": "<Type of request>"
        "commit": [
            "01ab23cd....": "<BASE64: xml commit>"
        ]
        "object": [
            "01ab23cd....": "<BASE64: xml commit>"
        ]
    }
*/

//-------------------------------------------------------------------------------------------------

CAzeServerProcessor::CAzeServerProcessor(quint16 iPort)
    : CDynamicHTTPServer(iPort)
{
}

//-------------------------------------------------------------------------------------------------

CAzeServerProcessor::~CAzeServerProcessor()
{
}

//-------------------------------------------------------------------------------------------------

void CAzeServerProcessor::getContent(CWebContext& tContext, QString& sHead, QString& sBody, QString& sCustomResponse, QString& sCustomResponseMIME)
{
    Q_UNUSED(sHead);
    Q_UNUSED(sBody);

    if (not tContext.pathValid())
    {
        if (not tContext.m_baPostContent.isEmpty())
        {
            if (tContext.m_sContentType == MIME_Content_JSON)
            {
                CXMLNode xRequest = CXMLNode::parseJSON(QString(tContext.m_baPostContent));

                sCustomResponse = serveRequest(tContext, xRequest).toJsonString();
                sCustomResponseMIME = MIME_Content_JSON;
            }
        }
    }
    else
    {
        // CDynamicHTTPServer::getContent(tContext, sHead, sBody, sCustomResponse, sCustomResponseMIME);
    }
}

//-------------------------------------------------------------------------------------------------

CXMLNode CAzeServerProcessor::serveRequest(CWebContext& tContext, const CXMLNode& xRequest)
{
    CXMLNode xResult;
    QString sAction = xRequest.attributes()[PROPNAME_ACTION];

    if (not sAction.isEmpty())
    {
        // Get the IP
        QString sIP = tContext.m_sPeer;
    }

    return xResult;
}
//-------------------------------------------------------------------------------------------------
