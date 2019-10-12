
// Application
#include "CCommit.h"
#include "CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CCommit::CCommit(QObject* parent)
    : CObject(parent)
{
}

//-------------------------------------------------------------------------------------------------

CCommit::~CCommit()
{
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommit::clone() const
{
    CCommit* pResult = new CCommit();

    pResult->setAuthor(m_sAuthor);
    pResult->setDate(m_sDate);
    pResult->setMessage(m_sMessage);
    pResult->setUser(m_mUser);
    pResult->setFiles(m_mFiles);

    return pResult;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CCommit::toNode() const
{
    OUT_DEBUG(m_sId);

    CXMLNode xNode(CStrings::s_sParamCommit);

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
    QStringList lFiles;
    for (QString sId : m_mFiles.keys())
    {
        QString sText = CUtils::packIdAndFile(sId, m_mFiles[sId]);;
        OUT_DEBUG(sText);
        lFiles << sText;
    }
    xFiles.setValue(lFiles.join(CStrings::s_sNewLine));
    xNode << xFiles;

    return xNode;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::toFile(const QString& sFileName) const
{
    OUT_DEBUG(sFileName);

    toNode().save(sFileName);
    return true;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::addFile(QString sRelativeFileName, QString sId)
{
    OUT_DEBUG(sRelativeFileName);
    OUT_DEBUG(sId);

    if (not m_mFiles.values().contains(sRelativeFileName))
    {
        if (sId.isEmpty())
            sId = CUtils::idFromFile(sRelativeFileName);

        m_mFiles[sId] = sRelativeFileName;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::removeFile(QString sRelativeFileName)
{
    OUT_DEBUG(sRelativeFileName);
    return true;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::add(const QString& sRootPath, const QString& sObjectPath, CCommit* pCommitToAdd)
{
    for (QString sAddKey : pCommitToAdd->m_mFiles.keys())
    {
        QString sRelativeFileName = pCommitToAdd->m_mFiles[sAddKey];
        QString sAbsoluteFileName = CUtils::absoluteFileName(sRootPath, sRelativeFileName);

        OUT_DEBUG(QString("sAbsoluteFileName=%1").arg(sAbsoluteFileName));

        if (CUtils::fileExists(sRootPath, sRelativeFileName))
        {
            // This file exists in working directory

            QString sExistingId;
            QList<QString> lKeys = m_mFiles.keys(sRelativeFileName);
            if (lKeys.count() > 0)
                sExistingId = lKeys[0];

            if (sExistingId.isEmpty())
            {
                // File object does not exist

                QString sNewObjectId = CUtils::storeFileInDB(sObjectPath, sAbsoluteFileName);

                if (not sNewObjectId.isEmpty())
                {
                    m_mFiles[sNewObjectId] = sRelativeFileName;
                }
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommit::fromNode(const CXMLNode& xNode)
{
    QDictionary mFiles;
    QString sId;
    QString sFilePath;

    CCommit* pCommit = new CCommit();

    CXMLNode xInfo = xNode.getNodeByTagName(CStrings::s_sParamInfo);
    pCommit->setAuthor(xInfo.attributes()[CStrings::s_sParamAuthor]);
    pCommit->setDate(xInfo.attributes()[CStrings::s_sParamDate]);

    CXMLNode xMessage = xNode.getNodeByTagName(CStrings::s_sParamMessage);
    pCommit->setMessage(xMessage.value());

    CXMLNode xUser = xNode.getNodeByTagName(CStrings::s_sParamUser);
    pCommit->setUser(CUtils::dictionaryFromNode(xUser));

    CXMLNode xFiles = xNode.getNodeByTagName(CStrings::s_sParamFiles);
    QStringList sLines = xFiles.value().split(CStrings::s_sNewLine);

    for (QString sLine : sLines)
    {
        CUtils::unpackIdAndFile(sLine, sId, sFilePath);

        if (not sId.isEmpty())
            mFiles[sId] = sFilePath;
    }
    pCommit->setFiles(mFiles);

    return pCommit;
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommit::fromFile(const QString& sFileName)
{
    return fromNode(CXMLNode::load(sFileName));
}

} // namespace Aze
