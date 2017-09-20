#include "libs.h"
#include "server.h"
#include "trafficQueue.h"
#include "logicThread.h"

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
    print("server started: " + std::string(host));
    int result = Application::run();
    print("server stopped");
    return result;
}

int Server::main(const std::vector<std::string>& args)
{
    TrafficQueue queue;
    bool logicThreadState = true;
    std::thread lt(logicThread, std::ref(queue), std::ref(logicThreadState));

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

        const int buffer_size = 1500;
        char buffer[buffer_size];

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
                for (auto &s : read) {
                    Net::DatagramSocket udp(s);
                    int avalible = udp.available();
                    int received = udp.receiveBytes(buffer, buffer_size);
                    queue.push(buffer, received);
                }
            }
            Thread::sleep(1);
        }
    }
    catch(Poco::Exception& e) 
    {
        result = e.code();
        print(parseError(e));
    }
    logicThreadState = false;
    lt.join();
    return result;
}

void Server::stop()
{
    workingMode = false;
    stopEvent.wait();
}

void Server::close()
{
    stopEvent.notify();
}
