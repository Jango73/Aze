
// Qt
#include <QFile>
#include <QDir>

// AzeLib
#include "CRepository.h"
#include "CUtils.h"
#include "CStrings.h"

// Application
#include "../client/CAzeClient.h"
#include "../common/CConstants.h"
#include "CTestAze.h"

//-------------------------------------------------------------------------------------------------
// Some macros to hide boiler plate code

#define COMMIT(a) \
exec({ \
    CConstants::s_sSwitchCommit, \
    QString("--%1").arg(CConstants::s_sSwitchSilent), \
    QString("--%1").arg(CConstants::s_sSwitchMessage), \
    a, \
    QString("--%1").arg(CConstants::s_sSwitchAuthor), \
    m_sAuthor \
    })

#define STAGE(...) \
    exec({ \
    CConstants::s_sSwitchStage, \
    QString("--%1").arg(CConstants::s_sSwitchSilent), \
    __VA_ARGS__ \
    })

#define DIFF_LAST(a) \
    a = exec({ \
    CConstants::s_sSwitchDiff, "tip~1 tip > " \
    })

#define CREATE_BRANCH(a) \
    exec({ \
    CConstants::s_sSwitchCreateBranch, \
    QString("--%1").arg(CConstants::s_sSwitchSilent), a \
    })

#define SWITCH_TO_BRANCH(a) \
    exec({ \
    CConstants::s_sSwitchSwitchToBranch, \
    QString("--%1").arg(CConstants::s_sSwitchAllowFileDelete), \
    QString("--%1").arg(CConstants::s_sSwitchSilent), a \
    })

#define MERGE_BRANCH(a, b) \
    a = exec({ \
    CConstants::s_sSwitchMerge, \
    QString("--%1").arg(CConstants::s_sSwitchSilent), \
    b \
    })

//-------------------------------------------------------------------------------------------------

CTestAze::CTestAze(const QString& sArgument0)
    : m_sArgument0(sArgument0)
    , m_iCommitIndex(0)
{
    m_sRootPath = QDir::currentPath();
    m_sDataPath = "./.aze";
    m_sAuthor = "CTestAze";
    m_sBranch1 = "branch1";
    m_sBranch2 = "branch2";

    m_sInfoPath = QString("%1/%2")
            .arg(m_sDataPath)
            .arg(Aze::CStrings::s_sGeneralInformationFileName);

    m_sTrunkPath = QString("%1/%2/%3.%4")
            .arg(m_sDataPath)
            .arg(Aze::CStrings::s_sPathAzeBranchPath)
            .arg(Aze::CStrings::s_sDefaultBranchName)
            .arg(Aze::CStrings::s_sCompressedXMLExtension);

    m_sBranch1Path = QString("%1/%2/%3.%4")
            .arg(m_sDataPath)
            .arg(Aze::CStrings::s_sPathAzeBranchPath)
            .arg(m_sBranch1)
            .arg(Aze::CStrings::s_sCompressedXMLExtension);

    m_sFilesFolderName = "Files";
    m_sFilesFolderPath = QString("%1/%2").arg(m_sRootPath).arg(m_sFilesFolderName);
}

//-------------------------------------------------------------------------------------------------

void CTestAze::clearRepository()
{
    // Remove repo
    QDir repo(m_sDataPath);
    if (repo.exists())
        repo.removeRecursively();

    // Remove files
    QDir files(m_sFilesFolderName);
    if (files.exists())
        files.removeRecursively();
}

//-------------------------------------------------------------------------------------------------

QMap<QString, QString> CTestAze::generateFiles(int iCount)
{
    QMap<QString, QString> mFiles;

    for (int i = 0; i < iCount; i++)
    {
        mFiles[QString("%1.txt").arg(m_tTextGenerator.word())] = m_tTextGenerator.text();
    }

    return mFiles;
}

//-------------------------------------------------------------------------------------------------

bool CTestAze::createFile(const QString& sName, const QString& sContent)
{
    QFile file(sName);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(sContent.toUtf8().constData());
        file.close();

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CTestAze::readFile(const QString& sName, QString& sContent)
{
    QFile file(sName);

    if (file.open(QIODevice::ReadOnly))
    {
        sContent = QString(file.readAll());
        file.close();

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void CTestAze::createManyFiles()
{
    QDir filesDir(m_sFilesFolderPath);

    for (int iIndex = 0; iIndex < 5; iIndex++)
    {
        // Create some directory
        QString sNewFolder = m_tTextGenerator.word();
        QString sNewFolderPath = QString("%1/%2").arg(m_sFilesFolderName).arg(sNewFolder);

        QDir newFolder(sNewFolderPath);
        if (not newFolder.exists())
            QVERIFY(filesDir.mkdir(sNewFolder));

        // Create some files
        lStage.clear();
        QMap<QString, QString> mNewFiles = generateFiles(20 + rand() % 30);

        for (QString sNewFile : mNewFiles.keys())
        {
            lStage << QString("%1/%2")
                      .arg(sNewFolderPath)
                      .arg(sNewFile);

            QVERIFY(createFile(lStage.last(), mNewFiles[sNewFile]));
        }

        lStage.prepend(CConstants::s_sSwitchStage);

        exec(lStage);
        exec({
                 CConstants::s_sSwitchCommit,
                 QString("--%1").arg(CConstants::s_sSwitchMessage),
                 QString("Commit%1\nMultiline commit").arg(m_iCommitIndex++),
                 QString("--%1").arg(CConstants::s_sSwitchAuthor),
                 m_sAuthor
             });
    }
}

//-------------------------------------------------------------------------------------------------

QString CTestAze::exec(const QStringList& lInputArguments)
{
    QStringList lFinalArguments(lInputArguments);
    QByteArray baOutput;
    QTextStream sOutput(&baOutput);

    lFinalArguments.prepend(m_sArgument0);
    int argc = lFinalArguments.count();
    char** argv = new char*[size_t(argc)];
    int index;

    // Add arguments
    for (index = 0; index < argc; index++)
    {
        argv[index] = new char[size_t(lFinalArguments[index].count() + 2)];
        strcpy(argv[index], lFinalArguments[index].toUtf8().constData());
    }

    // Instantiate and run aze
    int iErrorCode = CAzeClient(argc, argv, &sOutput).run();

    for (index = 0; index < argc; index++)
        delete [] argv[index];

    delete [] argv;

    // If output is empty, return the error code
    if (baOutput.count() == 0)
    {
        return QString::number(iErrorCode);
    }

    return QString::fromUtf8(baOutput);
}

//-------------------------------------------------------------------------------------------------

void CTestAze::checkCommitDiff(const QString& sDiff, QVector<CTransformedFile> vFiles)
{
    QList<QPair<QString, QString> > mFileDiffs = Aze::CUtils::splitDiff(sDiff);
    QVERIFY(mFileDiffs.count() == vFiles.count());

    for (CTransformedFile file : vFiles)
    {
        // Find the diff for this file
        auto sItFileDiff = std::find_if(mFileDiffs.begin(), mFileDiffs.end(), [=](QPair<QString, QString> o) { return o.first == file.m_sFilePath; });
        QVERIFY(sItFileDiff != mFileDiffs.end());
        // Apply the diff for this file
        QString sPatch = (*sItFileDiff).second;
        QString sFilePatched = Aze::CUtils::applyUnifiedDiff(file.m_sFileContent1, sPatch);
        QVERIFY(file.m_sFileContent2 == sFilePatched);
    }
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testAll()
{
    QVector<CTransformedFile> vTransformedFiles;
    QString sDiff;

    clearRepository();

    exec({ CConstants::s_sSwitchInitRepository });

    QFile trunk(m_sTrunkPath);
    QVERIFY(trunk.exists());

    // Create some files
    QString sFolder1Name = "Folder1";
    QString sFolder2Name = "Folder2";
    QString sFolder3Name = "Folder3";

    QString sFile1Path = "Files/File1.txt";
    QString sFile2Path = "Files/File2.txt";
    QString sFile3Path = "Files/Folder1/File3.txt";
    QString sFile4Path = "Files/Folder1/File4.txt";
    QString sFile5Path = "Files/Folder2/File5.txt";
    QString sFile6Path = "Files/Folder2/File6.txt";
    QString sFile7Path = "Files/Folder3/File7.txt";
    QString sFile8Path = "Files/Folder3/File8.txt";

    QString sFile1Content1 = "File1";
    QString sFile2Content1 = "File2";
    QString sFile3Content1 = "File3";
    QString sFile4Content1 = "File4";
    QString sFile5Content1 = "File5";
    QString sFile6Content1 = "File6";
    QString sFile7Content1 = "File7";
    QString sFile8Content1 = "File8";

    QString sFile1Content2 = "File1-bis";
    QString sFile2Content2 = "File2-bis";
    QString sFile3Content2 = "File3-bis";
    QString sFile4Content2 = "File4-bis";
    QString sFile5Content2 = "File5-bis";
    QString sFile6Content2 = "File6-bis";
    QString sFile7Content2 = "File7-bis";
    QString sFile8Content2 = "File8-bis";

    QDir rootDir(m_sRootPath);
    QVERIFY(rootDir.mkdir(m_sFilesFolderName));

    QDir filesDir(m_sFilesFolderPath);
    QVERIFY(filesDir.mkdir(sFolder1Name));
    QVERIFY(filesDir.mkdir(sFolder2Name));
    QVERIFY(filesDir.mkdir(sFolder3Name));

    // 1st commit
    QVERIFY(createFile(sFile1Path, sFile1Content1));
    QVERIFY(createFile(sFile2Path, sFile2Content1));
    QVERIFY(createFile(sFile3Path, sFile3Content1));
    QVERIFY(createFile(sFile4Path, sFile4Content1));

    exec({
             CConstants::s_sSwitchStage,
             sFile1Path,
             sFile2Path,
             sFile3Path,
             sFile4Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit1",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Modify files 1 and 2
    QVERIFY(createFile(sFile1Path, sFile1Content2));
    QVERIFY(createFile(sFile2Path, sFile2Content2));

    // 2nd commit
    exec({
             CConstants::s_sSwitchStage,
             sFile1Path,
             sFile2Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit2",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 1 and 2
    sDiff = exec({ CConstants::s_sSwitchDiff, "tip~1", "tip" });
    vTransformedFiles.clear();
    vTransformedFiles << CTransformedFile(sFile1Path, sFile1Content1, sFile1Content2);
    vTransformedFiles << CTransformedFile(sFile2Path, sFile2Content1, sFile2Content2);
    checkCommitDiff(sDiff, vTransformedFiles);

    // Modify files 3 and 4
    QVERIFY(createFile(sFile3Path, sFile3Content2));
    QVERIFY(createFile(sFile4Path, sFile4Content2));

    // 3rd commit
    exec({
             CConstants::s_sSwitchStage,
             sFile3Path,
             sFile4Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit3",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 2 and 3
    sDiff = exec({ CConstants::s_sSwitchDiff, "tip~1", "tip" });
    vTransformedFiles.clear();
    vTransformedFiles << CTransformedFile(sFile3Path, sFile3Content1, sFile3Content2);
    vTransformedFiles << CTransformedFile(sFile4Path, sFile4Content1, sFile4Content2);
    checkCommitDiff(sDiff, vTransformedFiles);

    // 4th commit
    QVERIFY(createFile(sFile5Path, sFile5Content1));
    QVERIFY(createFile(sFile6Path, sFile6Content1));

    exec({
             CConstants::s_sSwitchStage,
             sFile5Path,
             sFile6Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit4",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 3 and 4

    // Create branch 1
    exec({
             CConstants::s_sSwitchCreateBranch,
             m_sBranch1
         });

    QFile branch1(m_sBranch1Path);
    QVERIFY(branch1.exists());

    // Modify files 5 and 6
    QVERIFY(createFile(sFile5Path, sFile5Content2));
    QVERIFY(createFile(sFile6Path, sFile6Content2));

    // Switch to branch 1
    exec({
             CConstants::s_sSwitchSwitchToBranch,
             m_sBranch1,
             QString("--%1").arg(CConstants::s_sSwitchSilent)
         });

    // Here we have files 5 and 6 modified and unstaged

    // 5th commit
    exec({
             CConstants::s_sSwitchStage,
             sFile5Path,
             sFile6Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit5",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 4 and 5
    sDiff = exec({ CConstants::s_sSwitchDiff, "tip~1", "tip" });
    vTransformedFiles.clear();
    vTransformedFiles << CTransformedFile(sFile5Path, sFile5Content1, sFile5Content2);
    vTransformedFiles << CTransformedFile(sFile6Path, sFile6Content1, sFile6Content2);
    checkCommitDiff(sDiff, vTransformedFiles);

    // 6th commit
    QVERIFY(createFile(sFile7Path, sFile7Content1));
    QVERIFY(createFile(sFile8Path, sFile8Content1));

    exec({
             CConstants::s_sSwitchStage,
             sFile7Path,
             sFile8Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit6",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Modify files 7 and 8
    QVERIFY(createFile(sFile7Path, sFile7Content2));
    QVERIFY(createFile(sFile8Path, sFile8Content2));

    // 7th commit
    exec({
             CConstants::s_sSwitchStage,
             sFile7Path,
             sFile8Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit7",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 6 and 7
    sDiff = exec({
             CConstants::s_sSwitchDiff,
             "tip~1",
             "tip"
         });

    vTransformedFiles.clear();
    vTransformedFiles << CTransformedFile(sFile7Path, sFile7Content1, sFile7Content2);
    vTransformedFiles << CTransformedFile(sFile8Path, sFile8Content1, sFile8Content2);
    checkCommitDiff(sDiff, vTransformedFiles);

    // Switch to trunk
    exec({
             CConstants::s_sSwitchSwitchToBranch,
             Aze::CStrings::s_sDefaultBranchName,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchAllowFileDelete),
         });

    // Merge Branch1 on trunk
    exec({
             CConstants::s_sSwitchMerge,
             m_sBranch1
         });

    // Commit the merge
    exec({
             CConstants::s_sSwitchStage,
             sFile7Path,
             sFile8Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchSilent),
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit8-Merge",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Check merged files content
    QString sMergedFile7Content;
    QString sMergedFile8Content;
    QVERIFY(readFile(sFile7Path, sMergedFile7Content));
    QVERIFY(readFile(sFile8Path, sMergedFile8Content));

    QVERIFY(sMergedFile7Content == sFile7Content2);
    QVERIFY(sMergedFile8Content == sFile8Content2);
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testMerge()
{
    clearRepository();

    exec({ CConstants::s_sSwitchInitRepository });

    QString sFile1Path = "Files/MergeTest.txt";
    QString sFile1Content;
    QString sDiffPath = "Files/Diff.txt";
    QString sDiffContent;
    QString sResult;

    QString sCA1("CA1\n--");
    QString sCA2("CA1\nCA2\n--");
    QString sCA3("CA1\nCA2\nCA3\n--");

    QString sCB1("CA1\nCA2\n--\nCB1\n--");
    QString sCB2("CA1\nCA2\n--\nCB1\nCB2\n--");
    QString sCB3("CA1\nCA2\n--\nCB1\nCB2\nCB3\n--");
    QString sCB4("CA1\nCA2\n--\nCB1\nCB2\nCB3\nCB4\n--");
    QString sCB5("CA1\nCA2\n--\nCB1\nCB2\nCB3\nCB4\nCB5\n--");

    QString sCC1("CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1");
    QString sCC2("CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1\nCC2");
    QString sCC3("CA1\nCA2\n--\nCB1\nCB2\nCB3\n--\nCC1\nCC2\nCC3");

    QString sMA4("CA1\nCA2\nCA3\n--\nCB1\nCB2\nCB3\n--");
    QString sMA5("CA1\nCA2\nCA3\n--\nCB1\nCB2\nCB3\n--\nCC1\nCC2");
    QString sMA6("CA1\nCA2\nCA3\n--\nCB1\nCB2\nCB3\nCB4\nCB5\n--\nCC1\nCC2");

    QString sMC4("CA1\nCA2\nCA3\n--\nCB1\nCB2\nCB3\nCB4\nCB5\n--\nCC1\nCC2\nCC3");

    QString sDA2("diff --aze Files/MergeTest.txt Files/MergeTest.txt\n--- Files/MergeTest.txt\n+++ Files/MergeTest.txt\n@@ -1,8 +1,12 @@\n CA1\n+.CA2\n --*--\n");

    QDir rootDir(m_sRootPath);
    QVERIFY(rootDir.mkdir(m_sFilesFolderName));

    // Branch A
    CREATE_BRANCH("A");
    SWITCH_TO_BRANCH("A");

    // CA1
    QVERIFY(createFile(sFile1Path, sCA1));
    STAGE(sFile1Path);
    COMMIT("CA1");

    // CA2
    QVERIFY(createFile(sFile1Path, sCA2));
    STAGE(sFile1Path);
    COMMIT("CA2");

    DIFF_LAST(sDiffContent);
    // QVERIFY(sDiffContent == sDA2);

    // Create branch B
    CREATE_BRANCH("B");

    // CA3
    QVERIFY(createFile(sFile1Path, sCA3));
    STAGE(sFile1Path);
    COMMIT("CA3");

    // Switch to branch B
    SWITCH_TO_BRANCH("B");

    // CB1
    QVERIFY(createFile(sFile1Path, sCB1));
    STAGE(sFile1Path);
    COMMIT("CB1");

    // CB2
    QVERIFY(createFile(sFile1Path, sCB2));
    STAGE(sFile1Path);
    COMMIT("CB2");

    // CB3
    QVERIFY(createFile(sFile1Path, sCB3));
    STAGE(sFile1Path);
    COMMIT("CB3");

    // Create branch C
    CREATE_BRANCH("C");

    // Switch to branch C
    SWITCH_TO_BRANCH("C");

    // CC1
    QVERIFY(createFile(sFile1Path, sCC1));
    STAGE(sFile1Path);
    COMMIT("CC1");

    // CC2
    QVERIFY(createFile(sFile1Path, sCC2));
    STAGE(sFile1Path);
    COMMIT("CC2");

    // Switch to branch A
    SWITCH_TO_BRANCH("A");

    // Merge B on A
    MERGE_BRANCH(sResult, "B");
    QVERIFY(sResult == "0");
    COMMIT("MA4");

    // Check merged file contents
    QVERIFY(readFile(sFile1Path, sFile1Content));
    QVERIFY(sFile1Content == sMA4);

    // Merge C on A
    MERGE_BRANCH(sResult, "C");
    QVERIFY(sResult == "0");
    COMMIT("MA5");

    // Check merged file contents
    QVERIFY(readFile(sFile1Path, sFile1Content));
    QVERIFY(sFile1Content == sMA5);

    // Switch to branch B
    SWITCH_TO_BRANCH("B");

    // CB4
    QVERIFY(createFile(sFile1Path, sCB4));
    STAGE(sFile1Path);
    COMMIT("CB4");

    // CB5
    QVERIFY(createFile(sFile1Path, sCB5));
    STAGE(sFile1Path);
    COMMIT("CB5");

    // Switch to branch C
    SWITCH_TO_BRANCH("C");

    // CC3
    QVERIFY(createFile(sFile1Path, sCC3));
    STAGE(sFile1Path);
    COMMIT("CC3");

    // Switch to branch A
    SWITCH_TO_BRANCH("A");

    // Merge B on A
    MERGE_BRANCH(sResult, "B");
    QVERIFY(sResult == "0");
    COMMIT("MA6");

    // Check merged file contents
    QVERIFY(readFile(sFile1Path, sFile1Content));
    QVERIFY(sFile1Content == sMA6);

    // Switch to branch C
    SWITCH_TO_BRANCH("C");

    // Merge A on C
    MERGE_BRANCH(sResult, "A");
    QVERIFY(sResult == "0");
    COMMIT("MC4");

    // Check merged file contents
    QVERIFY(readFile(sFile1Path, sFile1Content));
    QVERIFY(sFile1Content == sMC4);
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testHeavy()
{
//    clearRepository();

//    exec({ CConstants::s_sSwitchInitRepository });

//    QDir rootDir(m_sRootPath);
//    QVERIFY(rootDir.mkdir(m_sFilesFolderName));

//    createManyFiles();

//    exec({ CConstants::s_sSwitchCreateBranch, m_sBranch1 });
//    exec({ CConstants::s_sSwitchSwitchToBranch, m_sBranch1 });

//    createManyFiles();

//    exec({ CConstants::s_sSwitchCreateBranch, m_sBranch2 });
//    exec({ CConstants::s_sSwitchSwitchToBranch, m_sBranch2 });

//    createManyFiles();
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testFinalize()
{
//    clearRepository();
}

//-------------------------------------------------------------------------------------------------
