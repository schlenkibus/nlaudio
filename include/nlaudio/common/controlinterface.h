#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <list>
#include <vector>

#include <audio/audiofactory.h>

namespace Nl {

class ControlInterfaceException : public std::exception
{
public:
    ControlInterfaceException(std::string func, std::string file, int line, int errorNumber, std::string what);
    virtual const char* what() const noexcept;

private:
    std::string m_func;
    std::string m_file;
    std::string m_msg;
    int m_line;
    int m_errno;
};


typedef void (*command)(std::vector<std::string> args, JobHandle jobHandle, int sockfd);

struct CommandDescriptor {
    command func;
    std::string cmd;
};


class ControlInterface {

public:
    ControlInterface(JobHandle jobHandle);
    void start();
    void stop();

    void addCommand(const CommandDescriptor&cd);

private:
    static std::string read(int fd);
    static void handleRequest(int fd, ControlInterface *ptr);
    static void run(ControlInterface *ptr);
    bool m_isRunning;
    std::atomic<bool> m_terminateRequest;
    std::thread *m_thread;
    std::list<CommandDescriptor> m_commands;
    JobHandle m_jobHandle;
};

} // namespace Nl
