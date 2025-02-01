
#include <mfapi.h>

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
void Core_Startup()
{
    MFStartup(MF_VERSION);
}

// OK
void Core_Shutdown()
{
    MFShutdown();
}
