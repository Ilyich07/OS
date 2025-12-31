
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <signal.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

std::vector<pid_t> getPidsByName(const std::string& processName) {
    std::vector<pid_t> pids;
    std::string command = "pgrep -f \"" + processName + "\"";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return pids;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        pid_t pid = static_cast<pid_t>(atoi(buffer));
        if (pid > 0) pids.push_back(pid);
    }
    pclose(pipe);
    return pids;
}

void killByPid(pid_t pid) {
    if (pid <= 0) return;
    if (kill(pid, SIGKILL) == 0) {
        std::cout << "    Sent SIGKILL to " << pid << std::endl;
    } else {
        std::cout << "    Failed to kill " << pid << " (may be already gone)" << std::endl;
    }
}

void killProcessByName(const char* name) {
    std::cout << "Killing process by name: " << name << std::endl;
    auto pids = getPidsByName(name);
    if (pids.empty()) {
        std::cout << "  No processes found for '" << name << "'" << std::endl;
        return;
    }
    std::cout << "  Found " << pids.size() << " process(es)" << std::endl;
    for (pid_t p : pids) killByPid(p);
}

void killProcessesFromEnvironment() {
    const char* env = std::getenv("PROC_TO_KILL");
    if (!env) {
        std::cout << "PROC_TO_KILL not set" << std::endl;
        return;
    }
    std::cout << "PROC_TO_KILL: " << env << std::endl;
    std::stringstream ss(env);
    std::string token;
    while (std::getline(ss, token, ',')) {
        size_t a = token.find_first_not_of(" \t");
        size_t b = token.find_last_not_of(" \t");
        if (a==std::string::npos || b==std::string::npos) continue;
        std::string name = token.substr(a, b-a+1);
        if (!name.empty()) killProcessByName(name.c_str());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " --id PID  OR  " << argv[0] << " --name NAME\n";
        return 1;
    }

    killProcessesFromEnvironment();

    if (strcmp(argv[1], "--id") == 0) {
        pid_t pid = static_cast<pid_t>(atoi(argv[2]));
        if (pid > 0) {
            std::cout << "Killing by PID: " << pid << std::endl;
            killByPid(pid);
        } else {
            std::cerr << "Invalid PID: " << argv[2] << std::endl;
            return 2;
        }
    } else if (strcmp(argv[1], "--name") == 0) {
        killProcessByName(argv[2]);
    } else {
        std::cerr << "Unknown option: " << argv[1] << std::endl;
        return 3;
    }

    return 0;
}

