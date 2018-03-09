/***
  Copyright (c) 2018 Nonlinear Labs GmbH

  Authors: Pascal Huerst <pascal.huerst@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
***/

#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <list>
#include <vector>
#include <ostream>

#include <audio/audiofactory.h>

namespace Nl {

class ControlInterfaceException : public std::exception
{
public:
    ControlInterfaceException(std::string func, std::string file, int line, int errorNumber, std::string what);
    virtual const char* what() const noexcept;

private:
    void help(std::vector<std::string> args, JobHandle jobHandle, int sockfd);

    std::string m_func;
    std::string m_file;
    std::string m_msg;
    int m_line;
    int m_errno;
};

class ControlInterface;
typedef void (*command)(std::vector<std::string> args, JobHandle jobHandle, int sockfd, ControlInterface *ptr);

struct CommandDescriptor {
    command func;
    std::string cmd;
};

std::ostream& operator<<(std::ostream& lhs, CommandDescriptor const& rhs);


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
    static void help(std::vector<std::string> args, JobHandle jobHandle, int sockfd, ControlInterface *ptr);
    bool m_isRunning;
    std::atomic<bool> m_terminateRequest;
    std::thread *m_thread;
    std::list<CommandDescriptor> m_commands;
    JobHandle m_jobHandle;
};


} // namespace Nl
