#pragma once

class CommandBase {
public:
    virtual ~CommandBase() {}
    virtual int userId() = 0;
    virtual void run() = 0;
};
typedef std::shared_ptr<CommandBase> Command;

class CommandsFactory {
public:
    virtual Command createCommand(JSON::Object::Ptr json) = 0;
};
