#include "libs.h"
#include "usersSharedTable.h"

UserPtr UsersSharedTable::getUser(int id)
{
    std::lock_guard<std::mutex> lk(m_guard);
    iterator it = users.find(id);
    if (it == users.end()) {
        UserPtr user =  std::make_shared<User>();
        users[id] = user;
        return user;
    }    
    return it->second;
}

class AddCommand : public CommandBase {
public:
    AddCommand(UsersTable t, int id, int value): users(t), m_id(id), m_value(value) {}
    int userId() { return m_id; }
    void run() {
        UserPtr p = users->getUser(m_id);
        int result = p->add(m_value);
        std::cout << m_id << " (" << ((m_value>0) ? "+" : "") << m_value << ")= " << result << std::endl;
    }
private:
    UsersTable users;
    int m_id, m_value;
};

CommandsFactoryForSharedUsers::CommandsFactoryForSharedUsers(UsersTable t) : users(t)
{
}

Command CommandsFactoryForSharedUsers::createCommand(JSON::Object::Ptr json)
{
    std::string id = json->getValue<std::string>("id");
    std::string op = json->getValue<std::string>("op");
    if (id.empty() || op.empty())
        return Command();
    if (op == "add") {
        int user_id = std::atoi(id.c_str());
        if (user_id > 0)
            return std::make_shared<AddCommand>(users, user_id, 1);
    }
    if (op == "sub") {
        int user_id = std::atoi(id.c_str());
        if (user_id > 0)
            return std::make_shared<AddCommand>(users, user_id, -1);
    }
    return Command(); 
}