
// Application
#include "CCommit.h"
#include "CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CCommit::CCommit(QObject* parent) : CObject(parent)
{
}

//-------------------------------------------------------------------------------------------------

CCommit::~CCommit()
{
}

//-------------------------------------------------------------------------------------------------

CXMLNode CCommit::toNode() const
{
    CXMLNode xNode;

    CXMLNode xInfo(CStrings::s_sParamInfo);
    xInfo.attributes()[CStrings::s_sParamAuthor] = m_sAuthor;
    xInfo.attributes()[CStrings::s_sParamDate] = m_sDate;
    xNode << xInfo;

    CXMLNode xMessage(CStrings::s_sParamMessage);
    xMessage.setValue(m_sMessage);
    xNode << xMessage;

    CXMLNode xUser = CUtils::nodeFromDictionary(m_mUser);
    xNode << xUser;

    CXMLNode xFiles(CStrings::s_sParamFiles);
    xFiles.setValue(m_lFiles.join(CStrings::s_sNewLine));
    xNode << xFiles;

    return xNode;
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommit::fromNode(const CXMLNode& xNode)
{
    CCommit* pCommit = new CCommit();

    CXMLNode xInfo = xNode.getNodeByTagName(CStrings::s_sParamInfo);
    pCommit->setAuthor(xInfo.attributes()[CStrings::s_sParamAuthor]);
    pCommit->setDate(xInfo.attributes()[CStrings::s_sParamDate]);

    CXMLNode xMessage = xNode.getNodeByTagName(CStrings::s_sParamMessage);
    pCommit->setMessage(xMessage.value());

    CXMLNode xUser = xNode.getNodeByTagName(CStrings::s_sParamUser);
    pCommit->setUser(CUtils::dictionaryFromNode(xUser));

    CXMLNode xFiles = xNode.getNodeByTagName(CStrings::s_sParamFiles);
    pCommit->setFiles(xFiles.value().split(CStrings::s_sNewLine));

    return pCommit;
}

} // namespace Aze
