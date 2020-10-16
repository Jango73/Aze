
// Application
#include "CObject.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CObject::CObject(QObject* parent)
    : QObject(parent)
{
    setDateToNow();
}

//-------------------------------------------------------------------------------------------------

CObject::~CObject()
{
}

//-------------------------------------------------------------------------------------------------

QString CObject::shortId() const
{
    return m_sId.left(m_sId.count() / 2);
}

//-------------------------------------------------------------------------------------------------

void CObject::setId(QString value)
{
    m_sId = value;
    emit idChanged();
    emit shortIdChanged();
}

//-------------------------------------------------------------------------------------------------

QString CObject::generateId() const
{
    return CUtils::idFromString(toNode().toString());
}

//-------------------------------------------------------------------------------------------------

void CObject::setDateToNow()
{
    m_sDate = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
}

//-------------------------------------------------------------------------------------------------

CXMLNode CObject::toNode() const
{
    return CXMLNode();
}

//-------------------------------------------------------------------------------------------------

} // namespace Aze
