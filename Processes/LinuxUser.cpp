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
#include <fcntl.h>

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
    int result = system(cmd.c_str());
    
    if (result == -1) {
        return false;
    }
    
    return (WEXITSTATUS(result) == 0);
}


std::vector<pid_t> getPidsByName(const std::string& name) {
    std::vector<pid_t> out;
    std::string cmd = "pgrep -x \"" + name + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return out;
    
    char buf[128];
    while (fgets(buf, sizeof(buf), pipe)) {
        pid_t p = atoi(buf);
        if (p > 0 && isPidAlive(p)) {
            out.push_back(p);
        }
    }
    pclose(pipe);
    return out;
}

pid_t launchProcess(const char* prog, const char* const argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }

        execvp(prog, const_cast<char**>(argv));
        _exit(127);
    } else if (pid < 0) {
        std::cerr << "Failed to fork for " << prog << ": " << strerror(errno) << std::endl;
    }
    return pid;
}

pid_t launchProcess(const char* prog) {
    const char* const argv[] = {prog, NULL};
    return launchProcess(prog, argv);
}

bool runKiller(const char* opt, const char* val) {
    pid_t pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        
        execl("./LinuxKiller", "LinuxKiller", opt, val, (char*)NULL);
        _exit(127);
    } else if (pid < 0) {
        std::cerr << "Failed to fork for Killer: " << strerror(errno) << std::endl;
        return false;
    }
    
    int status = 0;
    waitpid(pid, &status, 0);
    sleep(2);
    
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
    
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

int main() {
    std::cout << "=== LINUX USER DEMO ===\n";

    setenv("PROC_TO_KILL", "firefox", 1);
    
    
    pid_t p1 = launchProcess("xclock");
    if (p1 > 0) {
        std::cout << "   Launched with PID: " << p1 << std::endl;
    } else {
        std::cout << "   ERROR: Failed to launch xclock\n";
    }
    sleep(2);
    
   
    pid_t p2 = launchProcess("firefox");
    if (p2 > 0) {
        std::cout << "   Launched with PID: " << p2 << std::endl;
    } else {
        std::cout << "   ERROR: Failed to launch firefox\n";
    }
    sleep(2);


    bool xclock_alive = false;
    if (p1 > 0) {
        xclock_alive = isPidAlive(p1);
        std::cout << "   xclock PID " << p1 << " alive: " << (xclock_alive ? "YES" : "NO") << std::endl;
    } else {
        std::cout << "   xclock: Not launched\n";
    }
    
    bool second_alive = false;
    if (p2 > 0) {
        second_alive = isPidAlive(p2);
        std::cout << "   firefox PID " << p2 << " alive: " << (second_alive ? "YES" : "NO") << std::endl;
    } else {
        std::cout << "   firefox: Not launched\n";
    }

    // 5. Kill by name
    std::cout << "\n4) Kill by name 'xclock':\n";
    if (runKiller("--name", "xclock")) {
        std::cout << "   Success\n";
    } else {
        std::cout << "   Failed\n";
    }

    sleep(2);
    
    std::cout << "\n5) Check after killing by name:\n";
    if (p1 > 0) {
        xclock_alive = isPidAlive(p1);
        std::cout << "   xclock PID " << p1 << " alive: " << (xclock_alive ? "YES" : "NO") << std::endl;
    }
    
    if (p2 > 0) {
        second_alive = isPidAlive(p2);
        std::cout << "   firefox PID " << p2 << " alive: " << (second_alive ? "YES" : "NO") << std::endl;
    }

    std::cout << "\n6) === SIMPLE PID KILL DEMO ===\n";
    
    std::cout << "7) Launch new xclock...\n";
    pid_t demo_pid = launchProcess("xclock");
    
    if (demo_pid <= 0) {
        std::cout << "   ERROR: Cannot launch xclock\n";
        unsetenv("PROC_TO_KILL");
        return 1;
    }
    
    std::cout << "   Launched xclock with PID: " << demo_pid << std::endl;
    
    sleep(1);
    
    bool alive = isPidAlive(demo_pid);
    std::cout << "\n8) Check process is alive:\n";
    std::cout << "   PID " << demo_pid << " alive: " << (alive ? "YES" : "NO") << std::endl;
    
    if (!alive) {
        std::cout << "   ERROR: Process died unexpectedly\n";
        unsetenv("PROC_TO_KILL");
        return 1;
    }

    std::cout << "\n9) Kill by PID:\n";
    char pid_str[32];
    snprintf(pid_str, sizeof(pid_str), "%d", demo_pid);
    
    if (runKiller("--id", pid_str)) {
        std::cout << "   Success\n";
    } else {
        std::cout << "   Failed\n";
    }
    
    sleep(1);
    
    std::cout << "\n10) Final check:\n";
    bool still_alive = isPidAlive(demo_pid);
    std::cout << "   PID " << demo_pid << " alive: " << (still_alive ? "YES (FAIL)" : "NO (SUCCESS)") << std::endl;
    

    unsetenv("PROC_TO_KILL");
    
    
    system("pkill -f xclock 2>/dev/null");
    system("pkill -f firefox 2>/dev/null");
    
    std::cout << "\n=== DEMO COMPLETED ===\n";
    return 0;
}