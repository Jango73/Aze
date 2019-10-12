
// Application
#include "CStrings.h"

#define XMLC_EXTENSION  "xml"
// #define XMLC_EXTENSION  "xmlc"

namespace Aze {

const QString CStrings::s_sPathSep                  = "/";
const QString CStrings::s_sNewLine                  = "\n";
const QString CStrings::s_sNewLineEscaped           = "\\n";
const QString CStrings::s_sBase64Marker             = "B64:";
const QString CStrings::s_sPathIdSeparator          = "@";
const QString CStrings::s_sCompressedXMLExtension   = XMLC_EXTENSION;

const QString CStrings::s_sParamAuthor              = "author";
const QString CStrings::s_sParamBranch              = "branch";
const QString CStrings::s_sParamBranches            = "branches";
const QString CStrings::s_sParamCommit              = "commit";
const QString CStrings::s_sParamCurrent             = "current";
const QString CStrings::s_sParamDate                = "date";
const QString CStrings::s_sParamFiles               = "files";
const QString CStrings::s_sParamInfo                = "info";
const QString CStrings::s_sParamMessage             = "message";
const QString CStrings::s_sParamRoot                = "root";
const QString CStrings::s_sParamTip                 = "tip";
const QString CStrings::s_sParamType                = "type";
const QString CStrings::s_sParamUser                = "user";

const QString CStrings::s_sPathAzeDataRoot          = ".aze";
const QString CStrings::s_sPathAzeStashPath         = "stash";
const QString CStrings::s_sPathAzeBranchPath        = "branches";
const QString CStrings::s_sPathAzeCommitPath        = "commits";
const QString CStrings::s_sPathAzeObjectPath        = "objects";

const QString CStrings::s_sGeneralInfoFileName      = "info." XMLC_EXTENSION;
const QString CStrings::s_sStagingCommitFileName    = "stage." XMLC_EXTENSION;

const QString CStrings::s_sDefaultBranchName        = "trunk";

const QString CStrings::s_sTextNoSuchFile           = tr("No such file");
const QString CStrings::s_sTextNoCurrentBranch      = tr("No current branch");
const QString CStrings::s_sTextCommitEmpty          = tr("No files to commit");

} // namespace Aze
