#pragma once
#include "command.h"

class User {
public:
    User() : m_value(1) {}
    int add(int value) {
        m_value += value;
        return m_value;
    }
private:
    int m_value;
};
typedef std::shared_ptr<User> UserPtr;

class UsersSharedTable 
{
public:
    UserPtr getUser(int id);
private:
    std::map<int, UserPtr> users;
    typedef std::map<int, UserPtr>::iterator iterator;
    std::mutex m_guard;
};
typedef std::shared_ptr<UsersSharedTable> UsersTable;

class CommandsFactoryForSharedUsers : public CommandsFactory
{
public:
    CommandsFactoryForSharedUsers(UsersTable t);
    virtual Command createCommand(JSON::Object::Ptr json);
private:
    UsersTable users;
};
