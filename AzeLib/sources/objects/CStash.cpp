
// Application
#include "CStash.h"
#include "CDatabase.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CStash::CStash(QObject* parent)
    : CObject(parent)
    , m_pStage(nullptr)
{
    setDateToNow();
}

//-------------------------------------------------------------------------------------------------

CStash::~CStash()
{
}

//-------------------------------------------------------------------------------------------------

CXMLNode CStash::toNode() const
{
    CXMLNode xNode(CStrings::s_sParamStash);

    // Set general information
    CXMLNode xInfo(CStrings::s_sParamInfo);
    xInfo.attributes()[CStrings::s_sParamAuthor] = m_sAuthor;
    xInfo.attributes()[CStrings::s_sParamDate] = m_sDate;
    xNode << xInfo;

    // Set message
    CXMLNode xMessage(CStrings::s_sParamMessage);
    xMessage.setValue(m_sMessage);
    xNode << xMessage;

    // Set diff
    CXMLNode xDiff(CStrings::s_sParamDiff);
    xDiff.setValue(m_sDiff);
    xNode << xDiff;

    // Set stage
    if (IS_NOT_NULL(m_pStage))
    {
        CXMLNode xStage(CStrings::s_sParamStage);
        xStage << m_pStage->toNode();
        xNode << xStage;
    }

    return xNode;
}

//-------------------------------------------------------------------------------------------------

bool CStash::toFile(const QString& sFileName) const
{
    toNode().save(sFileName);
    return true;
}

//-------------------------------------------------------------------------------------------------

void CStash::setDateToNow()
{
    m_sDate = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
}

//-------------------------------------------------------------------------------------------------

CStash* CStash::fromNode(const CXMLNode& xNode, QObject* parent, QString sStashId)
{
    CStash* pStash = new CStash(parent);

    // Set the id
    pStash->setId(sStashId);

    // Read general information
    CXMLNode xInfo = xNode.getNodeByTagName(CStrings::s_sParamInfo);
    pStash->setAuthor(xInfo.attributes()[CStrings::s_sParamAuthor]);
    pStash->setDate(xInfo.attributes()[CStrings::s_sParamDate]);

    // Read message
    CXMLNode xMessage = xNode.getNodeByTagName(CStrings::s_sParamMessage);
    pStash->setMessage(xMessage.value());

    // Read diff
    CXMLNode xDiff = xNode.getNodeByTagName(CStrings::s_sParamDiff);
    pStash->setDiff(xDiff.value());

    // Read stage
    CXMLNode xStage = xNode.getNodeByTagName(CStrings::s_sParamStage);
    pStash->setStage(CCommit::fromNode(xStage.getNodeByTagName(CStrings::s_sParamCommit), pStash, CStrings::s_sParamStage));

    return pStash;
}

//-------------------------------------------------------------------------------------------------

CStash* CStash::fromFile(const QString& sFileName, QObject* parent, QString sStashId)
{
    return fromNode(CXMLNode::load(sFileName), parent, sStashId);
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
