#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>

bool isPidAlive(pid_t pid) {
    if (pid <= 0) return false;
    
    if (kill(pid, 0) == 0) {
        return true;
    }
    
    if (errno == ESRCH) {
        return false;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d", pid);
    struct stat sb;
    return (stat(path, &sb) == 0);
}

bool isProcessRunningByName(const std::string& name) {
    std::string cmd = "pgrep -x \"" + name + "\" > /dev/null 2>&1";
    return system(cmd.c_str()) == 0;
}

std::vector<pid_t> getPidsByName(const std::string& name) {
    std::vector<pid_t> out;
    std::string cmd = "pgrep -x \"" + name + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return out;
    
    char buf[128];
    while (fgets(buf, sizeof(buf), pipe)) {
        pid_t p = static_cast<pid_t>(atoi(buf));
        if (p > 0) {
            out.push_back(p);
        }
    }
    pclose(pipe);
    return out;
}

pid_t launchProcess(const char* prog) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(prog, prog, (char*)NULL);
        _exit(127); 
    }
    return pid;
}

bool runKiller(const char* opt, const char* val) {
    pid_t pid = fork();
    if (pid == 0) {

        execl("./LinuxKiller", "LinuxKiller", opt, val, (char*)NULL);
        _exit(127);
    }
    
    int status = 0;
    waitpid(pid, &status, 0);
    
    usleep(500000); // 500ms
    
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}


int main() {
    std::cout << "=== LINUX USER DEMO ===\n";

    setenv("PROC_TO_KILL", "xclock", 1);
    std::cout << "1) PROC_TO_KILL = xclock\n";

    std::cout << "\n2) Launching xclock...\n";
    pid_t p1 = launchProcess("xclock");
    if (p1 > 0) {
        std::cout << "   Launched xclock with PID: " << p1 << std::endl;
    } else {
        std::cout << "   Failed to launch xclock\n";
    }
    
    usleep(1000000); 

    std::cout << "\n3) BEFORE Killer check:\n";
    bool xclockRunning = isProcessRunningByName("xclock");
    std::cout << "   xclock running: " << (xclockRunning ? "YES" : "NO") << std::endl;

    // 4. Run Killer with --name
    std::cout << "\n4) Running Killer --name firefox" << std::endl;
    if (runKiller("--name", "firefox")) {
        std::cout << "   Killer finished successfully" << std::endl;
    } else {
        std::cout << "   Killer failed" << std::endl;
    }

    usleep(1000000); 
    
    std::cout << "\n5) AFTER Killer check:\n";
    xclockRunning = isProcessRunningByName("xclock");
    std::cout << "   xclock running: " << (xclockRunning ? "YES" : "NO") << std::endl;

    std::cout << "\n6) Launch xclock again...\n";
    pid_t new_xclock = launchProcess("xclock");
    usleep(1000000);

    auto pids = getPidsByName("xclock");
    if (!pids.empty()) {
        pid_t target = pids[0];
        std::cout << "   Found xclock PID: " << target << std::endl;

        char pidstr[32];
        snprintf(pidstr, sizeof(pidstr), "%d", target);

        std::cout << "\n7) Running Killer --id " << pidstr << std::endl;
        if (runKiller("--id", pidstr)) {
            std::cout << "   Killer finished successfully" << std::endl;
        }
    }

    usleep(500000); 
    
    std::cout << "\n8) Final check:\n";
    bool anyXclock = isProcessRunningByName("xclock");
    std::cout << "   Any xclock running: " << (anyXclock ? "YES" : "NO") << std::endl;

    unsetenv("PROC_TO_KILL");
    std::cout << "\n9) Unset PROC_TO_KILL\n";

    std::cout << "\n=== DEMO COMPLETED ===\n";
    return 0;
}