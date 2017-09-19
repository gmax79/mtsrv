#pragma once

class Server : protected Application
{
public:
    Server();
    int  run(int argc, char* argv[]);
    void stop();
private:
    int main(const std::vector <std::string> & args);
    int workingMode;
};
