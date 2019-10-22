
// Application
#include "CBranch.h"
#include "../CUtils.h"

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
    CXMLNode xNode(CStrings::s_sParamBranch);

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

CBranch* CBranch::fromNode(const CXMLNode& xNode, QObject* parent)
{
    QDictionary mFiles;
    QString sId;
    QString sFilePath;

    CBranch* pBranch = new CBranch(parent);

    CXMLNode xInfo = xNode.getNodeByTagName(CStrings::s_sParamInfo);

    if (not xNode.isEmpty())
    {
        pBranch->setAuthor(xInfo.attributes()[CStrings::s_sParamAuthor]);
        pBranch->setDate(xInfo.attributes()[CStrings::s_sParamDate]);
        pBranch->setRootCommitId(xInfo.attributes()[CStrings::s_sParamRoot]);
        pBranch->setTipCommitId(xInfo.attributes()[CStrings::s_sParamTip]);

        QString sType = xInfo.attributes()[CStrings::s_sParamType];
        if (not sType.isEmpty())
            pBranch->setType(qStringToEnum(CEnums, EBranchType, sType));
    }

    return pBranch;
}

} // namespace Aze
