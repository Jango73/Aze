
#pragma once

// Global
#include "../Aze_global.h"

// Qt
#include <QtTest/QtTest>

// Application
#include "../CRepository.h"

//-------------------------------------------------------------------------------------------------

namespace Aze {

class AZE_SHARED_EXPORT CTestAze : public CObject
{
    Q_OBJECT

private:

    bool createFile(const QString& sName, const QString& sContent);

    bool readFile(const QString& sName, QString& sContent);

    void commit(const QStringList& lStage, const QString& sAuthor, const QString& sMessage);

private slots:

    void testAll();

};

}
