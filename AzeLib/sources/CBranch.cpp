
// Application
#include "CBranch.h"
#include "CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CBranch::CBranch(QObject* parent)
    : CObject(parent)
    , m_eType(CEnums::eBranch)
{
}

//-------------------------------------------------------------------------------------------------

CBranch::~CBranch()
{
}

//-------------------------------------------------------------------------------------------------

CXMLNode CBranch::toNode() const
{
    CXMLNode xNode;

    CXMLNode xInfo(CStrings::s_sParamInfo);
    xInfo.attributes()[CStrings::s_sParamAuthor] = m_sAuthor;
    xInfo.attributes()[CStrings::s_sParamDate] = m_sDate;
    xInfo.attributes()[CStrings::s_sParamType] = enumToQString(CEnums, EBranchType, m_eType);
    xInfo.attributes()[CStrings::s_sParamRoot] = m_sRootCommitId;
    xInfo.attributes()[CStrings::s_sParamTip] = m_sTipCommitId;
    xNode << xInfo;

    return xNode;
}

//-------------------------------------------------------------------------------------------------

CBranch* CBranch::fromNode(const CXMLNode& xNode)
{
    QDictionary mFiles;
    QString sId;
    QString sFilePath;

    CBranch* pBranch = new CBranch();

    CXMLNode xInfo = xNode.getNodeByTagName(CStrings::s_sParamInfo);
    pBranch->setAuthor(xInfo.attributes()[CStrings::s_sParamAuthor]);
    pBranch->setDate(xInfo.attributes()[CStrings::s_sParamDate]);
    pBranch->setType(qStringToEnum(CEnums, EBranchType, xInfo.attributes()[CStrings::s_sParamType]));
    pBranch->setRootCommitId(xInfo.attributes()[CStrings::s_sParamRoot]);
    pBranch->setTipCommitId(xInfo.attributes()[CStrings::s_sParamTip]);

    return pBranch;
}

} // namespace Aze
