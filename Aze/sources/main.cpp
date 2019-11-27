
// Application
#include "AzeApp.h"
#include "tests/CTestAze.h"

int main(int argc, char *argv[])
{
    if (argc > 1 && QString(argv[1]) == "run-tests")
    {
        CTestAze test(argv[0]);
        QTEST_SET_MAIN_SOURCE_PATH
        return QTest::qExec(&test, 0, nullptr);
    }

    return AzeApp(argc, argv).run();
}
