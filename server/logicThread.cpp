#include "libs.h"
#include "logicThread.h"
#include "command.h"
#include "usersQueue.h"
#include "usersSharedTable.h"

void usersThread(QueueForUsers &q, bool &workingState) {    
    std::chrono::milliseconds timeout(100);
    while (workingState) {
        Command c = q.pop(timeout);
        if (c) {
            std::async(std::launch::async, [&]{ c->run(); });
        }
    }
}

void logicThread(TrafficQueue &q, bool &workingState)
{   
    QueueForUsers users;
    UsersTable t = std::make_shared<UsersSharedTable>();
    std::shared_ptr<CommandsFactory> cf(new CommandsFactoryForSharedUsers(t));

    std::thread worker(usersThread, std::ref(users), std::ref(workingState));

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
        Command cmd = cf->createCommand(object);
        if (!cmd) {
            //todo add log
            continue;
        }
        users.push(cmd);              
    }
    worker.join();
}