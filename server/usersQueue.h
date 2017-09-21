#pragma once

#include "command.h"

class QueueForUsers {
    friend class QueueCommandWrapper;
public:
    QueueForUsers() : m_lastUser(-1) {}
    void push(Command c);
    Command pop(std::chrono::milliseconds waittime);
private:
    Command trypop();
    void done(Command c);
    struct queue {
        queue() : locked(false) {}
        std::list<Command> commands;
        bool locked;
    };
    typedef std::shared_ptr<queue> commands_queue;
    std::map<int, commands_queue> m_usersQueues;
    typedef std::map<int, commands_queue>::iterator queue_iterator;
    std::mutex m_queueGuard;    
    std::condition_variable m_event;
    int m_lastUser;
};
