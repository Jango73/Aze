
// Qt
#include <QFile>
#include <QDir>

// Application
#include "CTestAze.h"
#include "../CRepository.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------

#define CREATE_REPO  { if (pRepository != nullptr) delete pRepository; pRepository = new CRepository(sRootPath, this); }
#define FINALIZE_REPO { if (pRepository != nullptr) delete pRepository; }

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
    CRepository* pRepository = nullptr;
    QString sRootPath = QDir::currentPath();

    CREATE_REPO;
    QVERIFY(pRepository->readStage());
    QVERIFY(pRepository->stage(lStage));
    QVERIFY(pRepository->writeStage());

    CREATE_REPO;
    QVERIFY(pRepository->readStage());
    QVERIFY(pRepository->commit(sAuthor, sMessage));
    QVERIFY(pRepository->writeCurrentBranch());
    QVERIFY(pRepository->clearStage());
    QVERIFY(pRepository->writeStage());

    FINALIZE_REPO;
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testAll()
{
    CRepository* pRepository = nullptr;
    QStringList lStage;

    QString sRootPath = QDir::currentPath();
    QString sRepoPath = "./.aze";

    QString sInfoPath = QString("%1/%2")
            .arg(sRepoPath)
            .arg(CStrings::s_sGeneralInfoFileName);

    QString sTrunkPath = QString("%1/%2/%3.%4")
            .arg(sRepoPath)
            .arg(CStrings::s_sPathAzeBranchPath)
            .arg(CStrings::s_sDefaultBranchName)
            .arg(CStrings::s_sCompressedXMLExtension);

    CREATE_REPO;
    QVERIFY(pRepository->init());

    QFile trunk(sTrunkPath);
    QVERIFY(trunk.exists());

    // Create some files
    QString sFilesFolderName = "Files";
    QString sFilesFolderPath = QString("%1/%2").arg(sRootPath).arg(sFilesFolderName);
    QString sFolder1Name = "Folder1";

    QString sFile1Path = "./Files/File1.txt";
    QString sFile2Path = "./Files/File2.txt";
    QString sFile3Path = "./Files/Folder1/File3.txt";
    QString sFile4Path = "./Files/Folder1/File4.txt";

    QDir rootDir(sRootPath);
    QVERIFY(rootDir.mkdir(sFilesFolderName));

    QDir filesDir(sFilesFolderPath);
    QVERIFY(filesDir.mkdir(sFolder1Name));

    QVERIFY(createFile(sFile1Path, "File1"));
    QVERIFY(createFile(sFile2Path, "File2"));
    QVERIFY(createFile(sFile3Path, "File3"));
    QVERIFY(createFile(sFile4Path, "File4"));

    // 1st commit
    lStage.clear();
    lStage << sFile1Path;
    lStage << sFile2Path;
    lStage << sFile3Path;
    lStage << sFile4Path;

    commit(lStage, "CTestAze", "Commit1");

    // Modify files
    QVERIFY(createFile(sFile1Path, "File1-bis"));
    QVERIFY(createFile(sFile2Path, "File2-bis"));

    // 2nd commit
    lStage.clear();
    lStage << sFile1Path;
    lStage << sFile2Path;

    commit(lStage, "CTestAze", "Commit2");

    // Diff commits 1 and 2
    QString sExpectedDiff = QStringList
            ({
                 "diff --aze Files/File2.txt Files/File2.txt",
                 "@@ -1,1 +1,1 @@",
                 "-File2",
                 "+File2-bis",
                 "diff --aze Files/File1.txt Files/File1.txt",
                 "@@ -1,1 +1,1 @@",
                 "-File1",
                 "+File1-bis"
             }).join(CStrings::s_sNewLine) + CStrings::s_sNewLine;

    CREATE_REPO;
    QVERIFY(pRepository->readStage());
    QString sActualDiff = pRepository->diff("root", "tip");
    QVERIFY(sActualDiff == sExpectedDiff);

    // Modify files
    QVERIFY(createFile(sFile3Path, "File3-bis"));
    QVERIFY(createFile(sFile4Path, "File4-bis"));

    // 3rd commit
    lStage.clear();
    lStage << sFile3Path;
    lStage << sFile4Path;

    commit(lStage, "CTestAze", "Commit3");

    // Diff commits 2 and 3
    sExpectedDiff = QStringList
            ({
                 "diff --aze Files/Folder1/File4.txt Files/Folder1/File4.txt",
                 "@@ -1,1 +1,1 @@",
                 "-File4",
                 "+File4-bis",
                 "diff --aze Files/Folder1/File3.txt Files/Folder1/File3.txt",
                 "@@ -1,1 +1,1 @@",
                 "-File3",
                 "+File3-bis"
             }).join(CStrings::s_sNewLine) + CStrings::s_sNewLine;

    CREATE_REPO;
    QVERIFY(pRepository->readStage());
    sActualDiff = pRepository->diff("tip~1", "tip");
    QVERIFY(sActualDiff == sExpectedDiff);

    //
    FINALIZE_REPO;

    // Remove repo
    QDir repo(sRepoPath);
    repo.removeRecursively();

    // Remove files
    QDir files(sFilesFolderName);
    files.removeRecursively();
}

//-------------------------------------------------------------------------------------------------

}
