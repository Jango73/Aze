
#pragma once

// Qt
#include <QtTest/QtTest>

// qt-plus
#include "CTextGenerator.h"

// Application
#include "CRepository.h"

//-------------------------------------------------------------------------------------------------

class CTestAze : public QObject
{
    Q_OBJECT

public:

    CTestAze(const QString& sArgument0);

private:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    void clearRepository();

    QMap<QString, QString> generateFiles(int iCount);

    bool createFile(const QString& sName, const QString& sContent);

    bool readFile(const QString& sName, QString& sContent);

    void createManyFiles();

    QString exec(const QStringList& lInputArguments);

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

private slots:

    void testAll();

    void testHeavy();

    void testFinalize();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    CTextGenerator m_tTextGenerator;
    QStringList lStage;
    QString m_sArgument0;
    QString m_sRootPath;
    QString m_sDataPath;
    QString m_sAuthor;
    QString m_sInfoPath;
    QString m_sTrunkPath;
    QString m_sBranch1Path;
    QString m_sFilesFolderName;
    QString m_sFilesFolderPath;
    QString m_sBranch1;
    QString m_sBranch2;
    int m_iCommitIndex;
};

//-------------------------------------------------------------------------------------------------
