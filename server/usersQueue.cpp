#include "libs.h"
#include "usersQueue.h"

class QueueCommandWrapper : public CommandBase {
public:
    QueueCommandWrapper(Command c, QueueForUsers *queue);
    virtual ~QueueCommandWrapper() {}
    int userId();
    void run();
private:
    Command realCommand;
    QueueForUsers *cmdOwner;
};

QueueCommandWrapper::QueueCommandWrapper(Command c, QueueForUsers *queue) : realCommand(c), cmdOwner(queue) 
{
}

int QueueCommandWrapper::userId() 
{
    return realCommand->userId(); 
}

void QueueCommandWrapper::run() 
{
    realCommand->run();
    cmdOwner->done(realCommand);
}

void QueueForUsers::push(Command c)
{
    int id = c->userId();
    std::lock_guard<std::mutex> lk(m_queueGuard);
    queue_iterator it = m_usersQueues.find(id);
    if (it == m_usersQueues.end()) {
        commands_queue list = std::make_shared<queue>();
        list->commands.push_back(c);
        m_usersQueues[id] = list;
        m_event.notify_one();
        return;
    }
    it->second->commands.push_back(c);
    if (!it->second->locked) {
        m_event.notify_one();
    }
}

Command QueueForUsers::pop(std::chrono::milliseconds waittime)
{
    Command c = trypop();
    if (c)
        return c;
    std::unique_lock<std::mutex> lk(m_queueGuard);
    if (m_event.wait_for(lk, waittime) == std::cv_status::timeout)
        return c;
    lk.unlock();
    return trypop();
}

Command QueueForUsers::trypop()
{
    std::lock_guard<std::mutex> lk(m_queueGuard);
    queue_iterator it = m_usersQueues.find(m_lastUser);
    queue_iterator it_end = m_usersQueues.end();
    if (it != it_end)
        it++;
    if (it == it_end)
        it = m_usersQueues.begin();
    for (; it != it_end; ++it) {
        if (it->second->locked || it->second->commands.empty())
            continue;
        m_lastUser = it->first;
        it->second->locked = true;
        Command realCommand = it->second->commands.front();
        Command c = std::make_shared<QueueCommandWrapper>(realCommand, this);
        return c;
    }   
    m_lastUser = -1;
    return Command();
}

void QueueForUsers::done(Command c)
{
    int id = c->userId();
    std::lock_guard<std::mutex> lk(m_queueGuard);
    queue_iterator it = m_usersQueues.find(id);
    if (it == m_usersQueues.end()) {
        assert(false);
        return;
    }
    it->second->commands.pop_front();
    it->second->locked = false;
}
