
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

int CAzeClient::push()
{
    ERROR_WHEN_FALSE(isASainRepository(), CConstants::s_iError_NotARepository);

    CXMLNode xRequest = Aze::CRemoteRepository(m_pRepository).getPushRequest();

    return CConstants::s_iError_None;
}

//-------------------------------------------------------------------------------------------------

void CAzeClient::postNetworkRequest(CXMLNode xRequest)
{
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
    }
    else
    {
    }

    m_bNetworkAccessFinished = true;
}

//-------------------------------------------------------------------------------------------------
