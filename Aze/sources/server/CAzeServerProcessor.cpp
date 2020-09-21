
// Qt
#include <QDebug>

// Aze
#include "CUtils.h"
#include "CStrings.h"

// Application
#include "CAzeServerProcessor.h"
#include "CAzeServer.h"

/*!
    \class CAzeServer
    \inmodule Aze
    \section1 General
*/

//-------------------------------------------------------------------------------------------------

/*
    AZE Request format

    {
        "branch": "<Name of branch>"
        "request": "<Type of request>"
        "branch": [
        ]
        "commit": [
            "01ab23cd....": "<BASE64: xml commit>"
        ]
        "object": [
            "01ab23cd....": "<BASE64: xml commit>"
        ]
    }
*/

//-------------------------------------------------------------------------------------------------

CAzeServerProcessor::CAzeServerProcessor(CAzeServer* parent, quint16 iPort)
    : CDynamicHTTPServer(iPort)
    , m_pServer(parent)
{
    m_mRequestFunctionMap[Aze::CStrings::s_sParamPull] = servePull;
    m_mRequestFunctionMap[Aze::CStrings::s_sParamPush] = servePush;
}

//-------------------------------------------------------------------------------------------------

CAzeServerProcessor::~CAzeServerProcessor()
{
}

//-------------------------------------------------------------------------------------------------

void CAzeServerProcessor::getContent(CWebContext& tContext, QString& sHead, QString& sBody, QString& sCustomResponse, QString& sCustomResponseMIME)
{
    Q_UNUSED(sHead)
    Q_UNUSED(sBody)

    if (tContext.pathValid() && not tContext.m_baPostContent.isEmpty())
    {
        if (tContext.m_sContentType == MIME_Content_JSON)
        {
            CXMLNode xRequest = CXMLNode::parseJSON(QString(tContext.m_baPostContent));

            // Compose repository name
            QString sRepositoryPath = QString("%1/%2")
                    .arg(m_pServer->repositoriesBasePath())
                    .arg(tContext.m_lPath.join(Aze::CStrings::s_sPathSep));

            Aze::CRepository repository(sRepositoryPath);

            if (repository.ok())
            {
                sCustomResponse = serveRequest(tContext, xRequest).toJsonString();
                sCustomResponseMIME = MIME_Content_JSON;
            }
            else
            {
                CXMLNode xResult;
                xResult.attributes()[Aze::CStrings::s_sParamResult] = Aze::CStrings::s_sParamError;
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
    QString sAction = xRequest.attributes()[Aze::CStrings::s_sParamRequest];

    if (not sAction.isEmpty())
    {
        // Get the IP
//        QString sIP = tContext.m_sPeer;

        if (IS_NOT_NULL(m_mRequestFunctionMap[sAction]))
        {
            return m_mRequestFunctionMap[sAction](tContext, xRequest);
        }
    }

    return xResult;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CAzeServerProcessor::servePull(CWebContext& tContext, const CXMLNode& xRequest)
{
    Q_UNUSED(tContext)

    CXMLNode xResult;

    xResult.attributes()[Aze::CStrings::s_sParamResult] = Aze::CStrings::s_sParamSuccess;

    QVector<Aze::CBranch*> lBranches;
    CXMLNodeList lNodes = xRequest.getNodesByTagName(Aze::CStrings::s_sParamBranch);

    // TODO: implement

    return xResult;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CAzeServerProcessor::servePush(CWebContext& tContext, const CXMLNode& xRequest)
{
    Q_UNUSED(tContext)
    Q_UNUSED(xRequest)

    CXMLNode xResult;

    xResult.attributes()[Aze::CStrings::s_sParamResult] = Aze::CStrings::s_sParamSuccess;

    // TODO: implement

    return xResult;
}

//-------------------------------------------------------------------------------------------------
