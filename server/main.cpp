#include "server.h"
#include "libs.h"

#if defined(POCO_OS_FAMILY_WINDOWS)
std::function<void()> stopHandler;
BOOL ConsoleCtrlHandler(DWORD ctrlType)
{
   if (ctrlType == CTRL_C_EVENT || ctrlType == CTRL_CLOSE_EVENT || ctrlType == CTRL_BREAK_EVENT) {
       stopHandler();
       return TRUE;
   }
   return FALSE;
}

void setStopGameHandlers(std::function<void()> f)
{
    stopHandler = f;
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleCtrlHandler, TRUE);
}

void clearStopGameHandlers()
{    
}
#else
#endif

int main(int argc, char* argv[])
{
    Server s;
    setStopGameHandlers(std::bind(&Server::stop, &s));
    int result = s.run(argc, argv);
    clearStopGameHandlers();
    s.close();
    return result;
}
