
// Qt
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

// qt-plus
#include <CXMLNode.h>

// Aze
#include "CRemoteRepository.h"

// Application
#include "CAzeClient.h"

//-------------------------------------------------------------------------------------------------

static void msleep(unsigned long msecs)
{
    QMutex mutex;
    mutex.lock();

    QWaitCondition waitCondition;
    waitCondition.wait(&mutex, msecs);

    mutex.unlock(); // Not necessary since new mutex will always be created,
    // but since destroying locked mutex
    // is bringing undefined behavior, let's follow some ethics
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::pull()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    CXMLNode xRequest = Aze::CRemoteRepository(m_pRepository).getPullRequest();

    postNetworkRequest(xRequest);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

int CAzeClient::push()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    CXMLNode xRequest = Aze::CRemoteRepository(m_pRepository).getPushRequest();

    postNetworkRequest(xRequest);

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

void CAzeClient::postNetworkRequest(CXMLNode xRequest)
{
    if (IS_NOT_NULL(m_pRepository))
    {
        setupRemoteHost(m_pRepository->remoteHostInfo()->name());
    }

    m_pNetworkAccessManager->post(m_tNetworkRequest, xRequest.toJsonString().simplified().toUtf8());

    while (not m_bNetworkAccessFinished)
    {
        msleep(10);
        QCoreApplication::processEvents();
    }
}

//-------------------------------------------------------------------------------------------------

void CAzeClient::onNetworkAccessFinished(QNetworkReply* pReply)
{
    if (pReply->error() == QNetworkReply::NoError)
    {
        CXMLNode xResponse = CXMLNode::parseJSON(QString(pReply->readAll()));

        if (xResponse.attributes()[Aze::CStrings::s_sParamResult] == Aze::CStrings::s_sParamSuccess)
        {
            QString s;
        }
    }
    else
    {
    }

    m_bNetworkAccessFinished = true;
}

//-------------------------------------------------------------------------------------------------
