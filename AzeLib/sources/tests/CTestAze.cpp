
// Qt
#include <QFile>
#include <QDir>

// Application
#include "CTestAze.h"
#include "../CRepository.h"

namespace Aze {

//-------------------------------------------------------------------------------------------------
// Macros

#define CREATE_REPO  { if (m_pRepository != nullptr) delete m_pRepository; m_pRepository = new CRepository(m_sRootPath, this); }
#define FINALIZE_REPO { if (m_pRepository != nullptr) delete m_pRepository; }

//-------------------------------------------------------------------------------------------------

CTestAze::CTestAze()
    : m_pRepository(nullptr)
{
    m_sRootPath = QDir::currentPath();
    m_sRepoPath = "./.aze";
    m_sAuthor = "CTestAze";

    m_sInfoPath = QString("%1/%2")
            .arg(m_sRepoPath)
            .arg(CStrings::s_sGeneralInfoFileName);

    m_sTrunkPath = QString("%1/%2/%3.%4")
            .arg(m_sRepoPath)
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
    QDir repo(m_sRepoPath);
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

    QString sFile1Path = "./Files/File1.txt";
    QString sFile2Path = "./Files/File2.txt";
    QString sFile3Path = "./Files/Folder1/File3.txt";
    QString sFile4Path = "./Files/Folder1/File4.txt";
    QString sFile5Path = "./Files/Folder2/File5.txt";
    QString sFile6Path = "./Files/Folder2/File6.txt";

    QDir rootDir(m_sRootPath);
    QVERIFY(rootDir.mkdir(m_sFilesFolderName));

    QDir filesDir(m_sFilesFolderPath);
    QVERIFY(filesDir.mkdir(sFolder1Name));
    QVERIFY(filesDir.mkdir(sFolder2Name));

    QVERIFY(createFile(sFile1Path, "File1"));
    QVERIFY(createFile(sFile2Path, "File2"));
    QVERIFY(createFile(sFile3Path, "File3"));
    QVERIFY(createFile(sFile4Path, "File4"));
    QVERIFY(createFile(sFile5Path, "File5"));
    QVERIFY(createFile(sFile6Path, "File6"));

    // 1st commit
    lStage.clear();
    lStage << sFile1Path;
    lStage << sFile2Path;
    lStage << sFile3Path;
    lStage << sFile4Path;

    commit(lStage, m_sAuthor, "Commit1");

    // Modify files
    QVERIFY(createFile(sFile1Path, "File1-bis"));
    QVERIFY(createFile(sFile2Path, "File2-bis"));

    // 2nd commit
    lStage.clear();
    lStage << sFile1Path;
    lStage << sFile2Path;

    commit(lStage, m_sAuthor, "Commit2");

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
    QVERIFY(m_pRepository->readStage());
    QString sActualDiff = m_pRepository->diff("root", "tip");
    QVERIFY(sActualDiff == sExpectedDiff);

    // Modify files
    QVERIFY(createFile(sFile3Path, "File3-bis"));
    QVERIFY(createFile(sFile4Path, "File4-bis"));

    // 3rd commit
    lStage.clear();
    lStage << sFile3Path;
    lStage << sFile4Path;

    commit(lStage, m_sAuthor, "Commit3");

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
    QVERIFY(m_pRepository->readStage());
    sActualDiff = m_pRepository->diff("tip~1", "tip");
    QVERIFY(sActualDiff == sExpectedDiff);

    // 4th commit
    lStage.clear();
    lStage << sFile5Path;
    lStage << sFile6Path;

    commit(lStage, m_sAuthor, "Commit4");

    // Diff commits 3 and 4
    sExpectedDiff = QStringList
            ({
                 "diff --aze Files/Folder2/File6.txt Files/Folder2/File6.txt",
                 "@@ -1,1 +1,1 @@",
                 "-",
                 "+File6",
                 "diff --aze Files/Folder2/File5.txt Files/Folder2/File5.txt",
                 "@@ -1,1 +1,1 @@",
                 "-",
                 "+File5"
             }).join(CStrings::s_sNewLine) + CStrings::s_sNewLine;

    CREATE_REPO;
    QVERIFY(m_pRepository->readStage());
    sActualDiff = m_pRepository->diff("tip~1", "tip");
    QVERIFY(sActualDiff == sExpectedDiff);
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testHeavy()
{
    clearRepository();

    CREATE_REPO;
    QVERIFY(m_pRepository->init());

    QDir rootDir(m_sRootPath);
    QVERIFY(rootDir.mkdir(m_sFilesFolderName));

    for (int iIndex = 0; iIndex < 50; iIndex++)
    {
        int iFileCount = 10 + rand() % 10;

        // Create some files
        QMap<QString, QString> mFiles = generateFiles(iFileCount);
        lStage.clear();

        for (QString sFile : mFiles.keys())
        {
            lStage << QString("%1/%2").arg(m_sFilesFolderName).arg(sFile);
            QVERIFY(createFile(QString("%1/%2").arg(m_sFilesFolderPath).arg(sFile), mFiles[sFile]));
        }

        commit(lStage, m_sAuthor, QString("Commit%1").arg(iIndex));
    }
}

//-------------------------------------------------------------------------------------------------

void CTestAze::testFinalize()
{
    FINALIZE_REPO;

//    clearRepository();
}

//-------------------------------------------------------------------------------------------------

}
