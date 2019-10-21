
// Application
#include "CFile.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

CFile::CFile(QObject* parent)
    : QObject(parent)
    , m_eStatus(CEnums::eLoose)
{
}

//-------------------------------------------------------------------------------------------------

CFile::~CFile()
{
}

} // namespace Aze
