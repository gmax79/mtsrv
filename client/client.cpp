#include "poco.h"
#include <sstream>

const char *host = "localhost:4444";

int main()
{
    Net::SocketAddress server(host);
    Net::DatagramSocket s;

    std::srand(unsigned(std::time(0)));
    std::string clientid(std::to_string( std::rand()));
    std::cout << "my id: " << clientid << std::endl;

    int packetCounter = 1;
    while (true) {

        std::stringstream json;
        json << "{ \"id\": \"" << clientid << "\", \"op\": \"";
        int op = std::rand();
        if ((op % 2) == 0)
        {
            json << "add";
        }
        else
        {
            json << "sub";
        }
        json << "\", \"packet\": \"" << packetCounter << "\" }";
        packetCounter++;
        std::string tosend(json.str());
        s.sendTo(tosend.c_str(), static_cast<int>(tosend.length()), server);
        std::cout << tosend << std::endl;
        Thread::sleep(500);
    }
    return 0;
}
