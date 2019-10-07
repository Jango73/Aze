
// Application
#include "AzeApp.h"

int main(int argc, char *argv[])
{
    // Build Aze instance
    AzeApp* aze = new AzeApp(argc, argv);

    // Run Aze instance
    int iResult = aze->run();

    // Delete Aze instance
    delete aze;

    return iResult;
}
