
#pragma once

// Global
#include "../Aze_global.h"

// Qt
#include <QtTest/QtTest>

// qt-plus
#include "CTextGenerator.h"

// Application
#include "../CRepository.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CTestAze : public QObject
{
    Q_OBJECT

public:

    CTestAze();

private:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    void clearRepository();

    QMap<QString, QString> generateFiles(int iCount);

    bool createFile(const QString& sName, const QString& sContent);

    bool readFile(const QString& sName, QString& sContent);

    void commit(const QStringList& lStage, const QString& m_sAuthor, const QString& sMessage);

    void createManyFiles();

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
    CRepository* m_pRepository;
    QStringList lStage;
    QString m_sRootPath;
    QString m_sRepoPath;
    QString m_sAuthor;
    QString m_sInfoPath;
    QString m_sTrunkPath;
    QString m_sFilesFolderName;
    QString m_sFilesFolderPath;
    int m_iCommitIndex;
};

}
