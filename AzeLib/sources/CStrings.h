
#pragma once

// Global
#include "Aze_global.h"

// Qt
#include <QObject>
#include <QtGlobal>

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CStrings : public QObject
{
    Q_OBJECT

public:

    static const QString s_sPathSep;
    static const QString s_sNewLine;
    static const QString s_sNewLineEscaped;
    static const QString s_sBase64Marker;
    static const QString s_sPathIdSeparator;
    static const QString s_sItemSeparator;
    static const QString s_sCompressedXMLExtension;
    static const QString s_sDiffChunkHeader;

    static const QString s_sParamAuthor;
    static const QString s_sParamBranch;
    static const QString s_sParamBranches;
    static const QString s_sParamCommit;
    static const QString s_sParamCurrent;
    static const QString s_sParamDate;
    static const QString s_sParamDiff;
    static const QString s_sParamFiles;
    static const QString s_sParamId;
    static const QString s_sParamInfo;
    static const QString s_sParamIsMerge;
    static const QString s_sParamMessage;
    static const QString s_sParamParents;
    static const QString s_sParamPush;
    static const QString s_sParamRepository;
    static const QString s_sParamRequest;
    static const QString s_sParamRoot;
    static const QString s_sParamStage;
    static const QString s_sParamStash;
    static const QString s_sParamStashList;
    static const QString s_sParamTip;
    static const QString s_sParamType;
    static const QString s_sParamUser;

    static const QString s_sPathAzeBranchPath;
    static const QString s_sPathAzeCommitPath;
    static const QString s_sPathAzeDataRootPath;
    static const QString s_sPathAzeMergePath;
    static const QString s_sPathAzeObjectPath;
    static const QString s_sPathAzeStashPath;

    static const QString s_sGeneralInfoFileName;
    static const QString s_sStagingCommitFileName;

    static const QString s_sDefaultBranchName;

    static const QString s_sTextBranchAlreadyExists;
    static const QString s_sTextBranchNameEmpty;
    static const QString s_sTextCannotMergeSameCommits;
    static const QString s_sTextCommitEmpty;
    static const QString s_sTextCommonAncestor;
    static const QString s_sTextCouldNotApplyPatch;
    static const QString s_sTextMergeFailed;
    static const QString s_sTextMergeWorkingDirectoryNotClean;
    static const QString s_sTextNoCommitToMerge;
    static const QString s_sTextNoCommonAncestor;
    static const QString s_sTextNoCurrentBranch;
    static const QString s_sTextNoFileSpecified;
    static const QString s_sTextNoStagingCommit;
    static const QString s_sTextNoSuchBranch;
    static const QString s_sTextNoSuchFile;
    static const QString s_sTextNotARepository;
    static const QString s_sTextNothingToStash;
    static const QString s_sTextNoTipCommit;
    static const QString s_sTextStashPopFailed;
    static const QString s_sTextStashSaveFailed;
};

}
