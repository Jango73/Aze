
// Application
#include "CCommit.h"
#include "CDatabase.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CCommit::CCommit(QObject* parent)
    : CObject(parent)
    , m_sDate(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
{
}

//-------------------------------------------------------------------------------------------------

CCommit::~CCommit()
{
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommit::clone(QObject* parent) const
{
    CCommit* pResult = new CCommit(parent);

    pResult->setId(m_sId);
    pResult->setIsMerge(m_bIsMerge);
    pResult->setAuthor(m_sAuthor);
    pResult->setDate(m_sDate);
    pResult->setMessage(m_sMessage);
    pResult->setParents(m_lParents);
    pResult->setUser(m_mUser);
    pResult->setFiles(m_mFiles);

    return pResult;
}

//-------------------------------------------------------------------------------------------------

CXMLNode CCommit::toNode() const
{
    CXMLNode xNode(CStrings::s_sParamCommit);

    CXMLNode xInfo(CStrings::s_sParamInfo);
    xInfo.attributes()[CStrings::s_sParamAuthor] = m_sAuthor;
    xInfo.attributes()[CStrings::s_sParamDate] = m_sDate;
    xInfo.attributes()[CStrings::s_sParamParents] = m_lParents.join(",");
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
        lFiles << sText;
    }
    xFiles.setValue(lFiles.join(CStrings::s_sNewLine));
    xNode << xFiles;

    return xNode;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::toFile(const QString& sFileName) const
{
    toNode().save(sFileName);
    return true;
}

//-------------------------------------------------------------------------------------------------

void CCommit::clearParents()
{
    m_lParents.clear();
}

//-------------------------------------------------------------------------------------------------

void CCommit::addParent(const QString& sParentId)
{
    m_lParents << sParentId;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::addFile(QString sRelativeFileName, QString sId)
{
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
    QString sKey = mapKeyForValue(m_mFiles, sRelativeFileName);

    if (not sKey.isEmpty())
        m_mFiles.remove(sKey);

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CCommit::addCommit(CDatabase* pDatabase, CCommit* pCommitToAdd)
{
    for (QString sAddKey : pCommitToAdd->m_mFiles.keys())
    {
        QString sRelativeFileName = pCommitToAdd->m_mFiles[sAddKey];
        QString sAbsoluteFileName = pDatabase->absoluteFileName(sRelativeFileName);

        if (CUtils::fileExists(pDatabase->rootPath(), sRelativeFileName))
        {
            // This file exists in working directory
            QString sExistingId = mapKeyForValue(m_mFiles, sRelativeFileName);
            QString sNewId = CUtils::idFromFile(sAbsoluteFileName);

            if (sExistingId.isEmpty() || sExistingId != sNewId)
            {
                // File object does not exist
                QString sNewObjectId = pDatabase->storeFile(sAbsoluteFileName);

                if (not sNewObjectId.isEmpty())
                {
                    m_mFiles.remove(sExistingId);
                    m_mFiles[sNewObjectId] = sRelativeFileName;
                }
            }
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

QByteArray CCommit::fileContent(CDatabase* pDatabase, QString sFileName)
{
    QString sId = mapKeyForValue(m_mFiles, sFileName);

    if (pDatabase->fileWithIdExists(sId))
    {
        return pDatabase->getObject(sId);
    }

    QString sFullName = pDatabase->composeLocalFileName(sFileName);

    return CUtils::getBinaryFileContent(sFullName);
}

//-------------------------------------------------------------------------------------------------

CCommit* CCommit::fromNode(const CXMLNode& xNode, QObject* parent, QString sCommitId)
{
    QDictionary mFiles;
    QString sId;
    QString sFilePath;

    CCommit* pCommit = new CCommit(parent);

    pCommit->setId(sCommitId);

    CXMLNode xInfo = xNode.getNodeByTagName(CStrings::s_sParamInfo);
    pCommit->setAuthor(xInfo.attributes()[CStrings::s_sParamAuthor]);
    pCommit->setDate(xInfo.attributes()[CStrings::s_sParamDate]);

    QString sParents = xInfo.attributes()[CStrings::s_sParamParents];

    if (not sParents.isEmpty())
        pCommit->setParents(sParents.split(CStrings::s_sItemSeparator));

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

CCommit* CCommit::fromFile(const QString& sFileName, QObject* parent, QString sCommitId)
{
    return fromNode(CXMLNode::load(sFileName), parent, sCommitId);
}

//-------------------------------------------------------------------------------------------------

QList<CCommit*> CCommit::parentList(CDatabase* pDatabase, const CCommit* pCommit, QObject* parent)
{
    QList<CCommit*> lParents;

    for (QString sParentId : pCommit->parents())
    {
        QString sCommitFileName = pDatabase->composeCommitFileName(sParentId);

        lParents << CCommit::fromFile(sCommitFileName, parent, sParentId);
    }

    return lParents;
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
