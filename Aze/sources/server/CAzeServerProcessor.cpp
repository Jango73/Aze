
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

    if (tContext.m_lPath.count() == 0 || tContext.m_lPath[0].isEmpty())
    {
        QString sResponse;

        if (tContext.m_baPostContent.isEmpty() == false)
        {
            CXMLNode xRequest = CXMLNode::parseJSON(QString(tContext.m_baPostContent));

            sCustomResponse = serveRequest(xRequest).toJsonString();
            sCustomResponseMIME = MIME_Content_JSON;
        }
    }
    else
    {
        // CDynamicHTTPServer::getContent(tContext, sHead, sBody, sCustomResponse, sCustomResponseMIME);
    }
}

//-------------------------------------------------------------------------------------------------

CXMLNode CAzeServerProcessor::serveRequest(CXMLNode xRequest)
{
    CXMLNode xResult;
    QString sAction = xRequest.attributes()[PROPNAME_ACTION];

    if (sAction.isEmpty() == false)
    {
//        // Get this user Id
//        qint64 iThisUserId = m_pServer->model()->idFromIP(m_tContext.m_sPeer);

//        if (sAction == ACTNAME_PING)
//            xResult = servePing(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_SET_OWN_PROFILE)
//            xResult = serveSetOwnProfile(xRequest);
//        else if (sAction == ACTNAME_SET_OWN_STATUS)
//            xResult = serveSetOwnStatus(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_GET_USER_LIST)
//            xResult = serveGetUserList(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_GET_USERS_BY_TAGS)
//            xResult = serveGetUsersByTags(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_REQUEST_TO_USER)
//            xResult = serveRequestToUser(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_RESPONSE_TO_USER)
//            xResult = serveResponseToUser(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_SET_MEETING)
//            xResult = serveSetMeeting(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_SET_GROUP)
//            xResult = serveSetGroup(xRequest, iThisUserId);
//        else if (sAction == ACTNAME_CHAT_MESSAGE)
//            xResult = serveChatMessage(xRequest, iThisUserId);
    }

    return xResult;
}
//-------------------------------------------------------------------------------------------------
