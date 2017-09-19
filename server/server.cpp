#include "stdlibs.h"
#include "poco.h"
#include "server.h"
#include "game.h"

const char* host = "127.0.0.1:4444";

std::string parseError(const Exception& e) {
    std::string error(e.what());
    const std::string& msg = e.message();
    if (!msg.empty()) {
        error.append(": ");
        error.append(msg);
    }
    return error;
}

void print(const std::string& error) {
    std::cout << error << std::endl;
}

Server::Server() : workingMode(true)
{
}

int Server::run(int argc, char* argv[])
{
    ///Application::setUnixOptions(true);
    try {
      Application::init(argc, argv);
    }
    catch (OptionException& e) {
      print(parseError(e));
      return EXIT_CONFIG;
    }
    return Application::run();
}

int Server::main(const std::vector<std::string>& args)
{
    Game game;
    int result = EXIT_OK;
    try {
        const long wait_mks = 200 * 1000;      
        Timespan timeout(0, wait_mks);
        std::vector<SharedPtr<Net::DatagramSocket>> listenSockets;

        Net::SocketAddress s(host);
        SharedPtr<Net::DatagramSocket> serverSocket;
        serverSocket.assign(new Net::DatagramSocket());
        serverSocket->setBlocking(false);
        serverSocket->bind(s, true);
        listenSockets.push_back(serverSocket);

        Net::Socket::SocketList read, write, error;
        while (workingMode)
        {
     		read.clear();
            std::for_each(listenSockets.begin(), listenSockets.end(),
            [&](SharedPtr<Net::DatagramSocket> s) { read.push_back(*s); });
            write.assign(read.begin(), read.end());
            error.assign(read.begin(), read.end());
            int totalSocketsToProcess = Net::Socket::select(read, write, error, timeout);
            if (totalSocketsToProcess > 0)
            {
                 


                 //todo working with sockets
            }
            Thread::sleep(1);
        }
    }
    catch(Poco::Exception& e) 
    {
        result = e.code();
        print(parseError(e));
    }
    return result;
}

void Server::stop()
{
    workingMode = false;
}
