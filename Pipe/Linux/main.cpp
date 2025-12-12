#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>


pid_t CreateChildProcess(const char* programName, 
                         int inputFd, 
                         int outputFd, 
                         const std::vector<int>& pipesToClose) {
    pid_t pid = fork();
    
    if (pid == 0) { 
        if (inputFd != STDIN_FILENO) {
            dup2(inputFd, STDIN_FILENO);
        }
        if (outputFd != STDOUT_FILENO) {
            dup2(outputFd, STDOUT_FILENO);
        }
        
        for (int pipeFd : pipesToClose) {
            close(pipeFd);
        }
       
        execl(programName, programName, NULL);
        
        
        std::cerr << "failed to execute " << programName << std::endl;
        exit(1);
    }
    
    return pid;
}

int main() {
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2];
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || 
        pipe(pipe3) == -1 || pipe(pipe4) == -1) {
        std::cerr << "failed to create pipes" << std::endl;
        return 1;
    }

    std::vector<int> allPipeFds = {
        pipe1[0], pipe1[1], pipe2[0], pipe2[1], 
        pipe3[0], pipe3[1], pipe4[0], pipe4[1]
    };

    pid_t pidM = CreateChildProcess("./M", pipe1[0], pipe2[1], allPipeFds);
    
    
    close(pipe1[0]);
    close(pipe2[1]);


    pid_t pidA = CreateChildProcess("./A", pipe2[0], pipe3[1], allPipeFds);
    
    close(pipe2[0]);
    close(pipe3[1]);

    pid_t pidP = CreateChildProcess("./P", pipe3[0], pipe4[1], allPipeFds);
    
    close(pipe3[0]);
    close(pipe4[1]);

    pid_t pidS = CreateChildProcess("./S", pipe4[0], STDOUT_FILENO,allPipeFds);
    
    close(pipe4[0]);

    const char* data = "1 2 3 4 5 6 7 8 9 10\n";
    write(pipe1[1], data, strlen(data));
    close(pipe1[1]);  


    waitpid(pidM, NULL, 0);
    waitpid(pidA, NULL, 0);
    waitpid(pidP, NULL, 0);
    waitpid(pidS, NULL, 0);

    return 0;
} 