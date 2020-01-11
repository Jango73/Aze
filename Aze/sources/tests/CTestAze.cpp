
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
            .arg(Aze::CStrings::s_sGeneralInfoFileName);

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
    CAzeClient(argc, argv, &sOutput).run();

    for (index = 0; index < argc; index++)
        delete argv[index];

    delete [] argv;

    return QString::fromUtf8(baOutput);
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testAll()
{
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

    QVERIFY(createFile(sFile1Path, sFile1Content1));
    QVERIFY(createFile(sFile2Path, sFile2Content1));
    QVERIFY(createFile(sFile3Path, sFile3Content1));
    QVERIFY(createFile(sFile4Path, sFile4Content1));
    QVERIFY(createFile(sFile5Path, sFile5Content1));
    QVERIFY(createFile(sFile6Path, sFile6Content1));

    // 1st commit
    exec({
             CConstants::s_sSwitchStage,
             sFile1Path,
             sFile2Path,
             sFile3Path,
             sFile4Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit1",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Modify files
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
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit2",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 1 and 2
    QString sDiff = exec({
             CConstants::s_sSwitchDiff,
             "root",
             "tip"
         });

//    QVERIFY(m_pRepository->readStage());
//    QList<QPair<QString, QString> > mFileDiffs = m_pRepository->splitDiff(m_pRepository->diff("root", "tip"));

//    QString sFile1Patched = CUtils::applyUnifiedDiff(sFile1Content1, mFileDiffs[sFile1Path]);
//    QString sFile2Patched = CUtils::applyUnifiedDiff(sFile2Content1, mFileDiffs[sFile2Path]);
//    QVERIFY(sFile1Content2 == sFile1Patched);
//    QVERIFY(sFile2Content2 == sFile2Patched);

    // Modify files
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
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit3",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 2 and 3
    // TODO

    // 4th commit
    exec({
             CConstants::s_sSwitchStage,
             sFile5Path,
             sFile6Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit4",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 3 and 4
    // TODO

    // Create branch 1
    exec({
             CConstants::s_sSwitchCreateBranch,
             m_sBranch1
         });

    QFile branch1(m_sBranch1Path);
    QVERIFY(branch1.exists());

    // Switch to branch 1
    exec({
             CConstants::s_sSwitchSwitchToBranch,
             m_sBranch1
         });

    // Create files
    QVERIFY(createFile(sFile7Path, sFile7Content1));
    QVERIFY(createFile(sFile8Path, sFile8Content1));

    // 5th commit
    exec({
             CConstants::s_sSwitchStage,
             sFile7Path,
             sFile8Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit5",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Diff commits 4 and 5
    // TODO

    // Modify files
    QVERIFY(createFile(sFile7Path, sFile7Content2));
    QVERIFY(createFile(sFile8Path, sFile8Content2));

    // 6th commit
    exec({
             CConstants::s_sSwitchStage,
             sFile7Path,
             sFile8Path
         });

    exec({
             CConstants::s_sSwitchCommit,
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit6",
             QString("--%1").arg(CConstants::s_sSwitchAuthor),
             m_sAuthor
         });

    // Switch to trunk
    exec({
             CConstants::s_sSwitchSwitchToBranch,
             Aze::CStrings::s_sDefaultBranchName,
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
             QString("--%1").arg(CConstants::s_sSwitchMessage),
             "Commit7",
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
