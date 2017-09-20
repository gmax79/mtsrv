#pragma once
#include "libs.h"

class Server : protected Application
{
public:
    Server();
    int  run(int argc, char* argv[]);
    void stop();
    void close();
private:
    int main(const std::vector <std::string> & args);
    int workingMode;
    SyncEvent stopEvent;
};
