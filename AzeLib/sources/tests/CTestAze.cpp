
// Qt
#include <QFile>
#include <QDir>

// Application
#include "CTestAze.h"
#include "../CRepository.h"
#include "../CUtils.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------
// Macros

#define CREATE_REPO  { if (m_pRepository != nullptr) delete m_pRepository; m_pRepository = new CRepository(m_sRootPath, this); }
#define FINALIZE_REPO { if (m_pRepository != nullptr) delete m_pRepository; }

//-------------------------------------------------------------------------------------------------

CTestAze::CTestAze()
    : m_pRepository(nullptr)
    , m_iCommitIndex(0)
{
    m_sRootPath = QDir::currentPath();
    m_sDataPath = "./.aze";
    m_sAuthor = "CTestAze";

    m_sInfoPath = QString("%1/%2")
            .arg(m_sDataPath)
            .arg(CStrings::s_sGeneralInfoFileName);

    m_sTrunkPath = QString("%1/%2/%3.%4")
            .arg(m_sDataPath)
            .arg(CStrings::s_sPathAzeBranchPath)
            .arg(CStrings::s_sDefaultBranchName)
            .arg(CStrings::s_sCompressedXMLExtension);

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

void CTestAze::commit(const QStringList& lStage, const QString& sAuthor, const QString& sMessage)
{
    QString sRootPath = QDir::currentPath();

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->stage(lStage));
    QVERIFY(m_pRepository->writeStage());

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->commit(sAuthor, sMessage));
    QVERIFY(m_pRepository->writeCurrentBranch());
    QVERIFY(m_pRepository->clearStage());
    QVERIFY(m_pRepository->writeStage());
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

        commit(lStage, m_sAuthor, QString("Commit%1\nMultiline commit").arg(m_iCommitIndex++));
    }
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testAll()
{
    clearRepository();

    CREATE_REPO;
    QVERIFY(m_pRepository->init());

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
    lStage.clear();
    lStage << sFile1Path;
    lStage << sFile2Path;
    lStage << sFile3Path;
    lStage << sFile4Path;

    commit(lStage, m_sAuthor, "Commit1");

    // Modify files
    QVERIFY(createFile(sFile1Path, sFile1Content2));
    QVERIFY(createFile(sFile2Path, sFile2Content2));

    // 2nd commit
    lStage.clear();
    lStage << sFile1Path;
    lStage << sFile2Path;

    commit(lStage, m_sAuthor, "Commit2");

    // Diff commits 1 and 2
    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QList<QPair<QString, QString> > mFileDiffs = m_pRepository->splitDiff(m_pRepository->diff("root", "tip"));

//    QString sFile1Patched = CUtils::applyUnifiedDiff(sFile1Content1, mFileDiffs[sFile1Path]);
//    QString sFile2Patched = CUtils::applyUnifiedDiff(sFile2Content1, mFileDiffs[sFile2Path]);
//    QVERIFY(sFile1Content2 == sFile1Patched);
//    QVERIFY(sFile2Content2 == sFile2Patched);

    // Modify files
    QVERIFY(createFile(sFile3Path, sFile3Content2));
    QVERIFY(createFile(sFile4Path, sFile4Content2));

    // 3rd commit
    lStage.clear();
    lStage << sFile3Path;
    lStage << sFile4Path;

    commit(lStage, m_sAuthor, "Commit3");

    // Diff commits 2 and 3
//    CREATE_REPO;
//    QVERIFY(m_pRepository->readStage());
//    sActualDiff = m_pRepository->diff("tip~1", "tip");
//    QString sFile3Patched = CUtils::applyUnifiedDiff(sFile3Content1, sActualDiff);
//    QString sFile4Patched = CUtils::applyUnifiedDiff(sFile4Content1, sActualDiff);
//    QVERIFY(sFile3Content2 == sFile3Patched);
//    QVERIFY(sFile4Content2 == sFile4Patched);

    // 4th commit
    lStage.clear();
    lStage << sFile5Path;
    lStage << sFile6Path;

    commit(lStage, m_sAuthor, "Commit4");

    // Diff commits 3 and 4
//    CREATE_REPO;
//    QVERIFY(m_pRepository->readStage());
//    sActualDiff = m_pRepository->diff("tip~1", "tip");
//    QString sFile5Patched = CUtils::applyUnifiedDiff(sFile5Content1, sActualDiff);
//    QString sFile6Patched = CUtils::applyUnifiedDiff(sFile6Content1, sActualDiff);
//    QVERIFY(sFile5Content2 == sFile5Patched);
//    QVERIFY(sFile6Content2 == sFile6Patched);

    // Create branch 1
    CREATE_REPO;
    QVERIFY(m_pRepository->createBranch("Branch1"));

    // Switch to branch 1
    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->switchToBranch("Branch1"));
    QVERIFY(m_pRepository->writeCurrentBranch());
    QVERIFY(m_pRepository->writeGeneralInfo());

    // Create files
    QVERIFY(createFile(sFile7Path, sFile7Content1));
    QVERIFY(createFile(sFile8Path, sFile8Content1));

    // 5th commit
    lStage.clear();
    lStage << sFile7Path;
    lStage << sFile8Path;

    commit(lStage, m_sAuthor, "Commit5");

//    // Diff commits 4 and 5
//    // TODO

    // Modify files
    QVERIFY(createFile(sFile7Path, sFile7Content2));
    QVERIFY(createFile(sFile8Path, sFile8Content2));

    // 6th commit
    lStage.clear();
    lStage << sFile7Path;
    lStage << sFile8Path;

    commit(lStage, m_sAuthor, "Commit6");

    // Switch to trunk
    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->switchToBranch("trunk", true));
    QVERIFY(m_pRepository->writeCurrentBranch());
    QVERIFY(m_pRepository->writeGeneralInfo());

    // Merge Branch1 on trunk
    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->merge("Branch1"));
    QVERIFY(m_pRepository->writeStage());

    // Commit the merge
    lStage.clear();
    lStage << sFile7Path;
    lStage << sFile8Path;

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->stage(lStage));
    QVERIFY(m_pRepository->writeStage());

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->commit(m_sAuthor, "Commit 7"));
    QVERIFY(m_pRepository->writeCurrentBranch());
    QVERIFY(m_pRepository->clearStage());
    QVERIFY(m_pRepository->writeStage());

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
    clearRepository();

    CREATE_REPO;
    QVERIFY(m_pRepository->init());

    QDir rootDir(m_sRootPath);
    QVERIFY(rootDir.mkdir(m_sFilesFolderName));

    createManyFiles();

    CREATE_REPO;
    QVERIFY(m_pRepository->createBranch("Branch1"));

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->switchToBranch("Branch1"));
    QVERIFY(m_pRepository->writeCurrentBranch());
    QVERIFY(m_pRepository->writeGeneralInfo());

    createManyFiles();

    CREATE_REPO;
    QVERIFY(m_pRepository->createBranch("Branch2"));

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    QVERIFY(m_pRepository->switchToBranch("Branch2"));
    QVERIFY(m_pRepository->writeCurrentBranch());
    QVERIFY(m_pRepository->writeGeneralInfo());

    createManyFiles();
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testFinalize()
{
    FINALIZE_REPO;

    clearRepository();
}

//-------------------------------------------------------------------------------------------------

}
