
// Application
#include "CEnums.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

QString CEnums::FileStatusSymbol(EFileStatus eStatus)
{
    switch (eStatus)
    {
    case eUnknown: return " ";
    case eLoose: return "~";
    case eClean: return " ";
    case eModified: return "*";
    case eAdded: return "+";
    case eDeleted: return "-";
    case eMissing: return "?";
    case eIgnored: return "!";
    case eAll: return " ";
    }

    return "";
}

//-------------------------------------------------------------------------------------------------

}
