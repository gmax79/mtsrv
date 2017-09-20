#include "libs.h"
#include "logicThread.h"

void logicThread(TrafficQueue &q, bool &workingState)
{   
    JSON::Parser parser;
    std::string data;
    std::chrono::milliseconds timeout(100);
    while (workingState) {
        if (!q.pop(data, timeout))        
         continue;        
        Dynamic::Var result;
        try {
            parser.reset();
            result = parser.parse(data);
        }
        catch (Poco::Exception&) {
            std::cout << "invalid json: " << data << std::endl;
            continue;
        }
        JSON::Object::Ptr object = result.extract<JSON::Object::Ptr>();
        std::string id = object->getValue<std::string>("id");
        std::string op = object->getValue<std::string>("op");
        std::cout << id << ", " << op << std::endl;
    }
}