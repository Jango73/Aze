
// Application
#include "CEnums.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

QString CEnums::FileStatusSymbol(EFileStatus eStatus)
{
    switch (eStatus)
    {
    case eLoose: return "?";
    case eClean: return " ";
    case eModified: return "M";
    case eAdded: return "+";
    case eDeleted: return "-";
    case eIgnored: return "!";
    }

    return "";
}

//-------------------------------------------------------------------------------------------------

}
