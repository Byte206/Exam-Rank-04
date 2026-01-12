#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int picoshell(char **cmds[])
// Function that receives an array of command arrays
// Example: cmds[0] = {"ls", "-l", NULL}, cmds[1] = {"grep", "txt", NULL}
{
    int i = 0;
    // Index to iterate through commands
    
    int fd[2];
    // Array to store pipe file descriptors: fd[0]=read end, fd[1]=write end
    
    int in_fd = 0;
    // File descriptor for the input of the current command
    // Initialized to 0 (stdin) for the first command
    
    int ret = 0;
    // Return value: 0 = success, 1 = error
    // Will be set to 1 if any child process fails
    
    pid_t pid;
    // Variable to store the child process ID after fork()
    
    int status;
    // Variable to store the exit status of child processes
    
    while (cmds[i])
    // Loop through each command until NULL is found
    {
        if (cmds[i + 1])
        // If there is a next command (not the last one)
        {
            if (pipe(fd) == -1)
                // Creates a pipe and checks if it failed
                return 1;
                // If pipe() fails, return error immediately
        }
        else
        // If this is the last command
        {
            fd[0] = -1;
            // Mark read end as invalid (no pipe needed)
            fd[1] = -1;
            // Mark write end as invalid (no pipe needed)
        }
        
        pid = fork();
        // Creates a child process
        // Returns 0 to child, child's PID to parent, -1 on error
        
        if (pid < 0)
        // If fork() failed
        {
            if (fd[0] != -1)
                close(fd[0]);
                // Close read end of current pipe if it exists
            if (fd[1] != -1)
                close(fd[1]);
                // Close write end of current pipe if it exists
            if (in_fd != 0)
                close(in_fd);
                // Close previous pipe's read end if it exists
            return 1;
            // Return error after cleaning up file descriptors
        }
        
        if (pid == 0)
        // Code executed by CHILD PROCESS
        {
            if (in_fd != 0)
            // If there's input from a previous pipe
            {
                if (dup2(in_fd, 0) == -1)
                    // Duplicate in_fd onto stdin (file descriptor 0)
                    // Checks if dup2() failed
                    exit(1);
                    // Exit child with error if dup2 failed
                close(in_fd);
                // Close original descriptor (now duplicated to stdin)
            }
            
            if (fd[1] != -1)
            // If there's an output pipe (not the last command)
            {
                if (dup2(fd[1], 1) == -1)
                    // Duplicate fd[1] onto stdout (file descriptor 1)
                    // Checks if dup2() failed
                    exit(1);
                    // Exit child with error if dup2 failed
                close(fd[1]);
                // Close original write end (now duplicated to stdout)
                close(fd[0]);
                // Close read end (child doesn't need it)
            }
            
            execvp(cmds[i][0], cmds[i]);
            // Replace process with the command
            // cmds[i][0] is the command name
            // cmds[i] is the full argument array
            // If successful, this never returns
            
            exit(1);
            // Only reached if execvp() failed
            // Exit with error code 1 (IMPORTANT: not 0!)
        }
        else
        // Code executed by PARENT PROCESS
        {
            if (in_fd != 0)
                close(in_fd);
                // Close previous pipe's read end (no longer needed)
            
            if (fd[1] != -1)
                close(fd[1]);
                // Close write end of current pipe (parent doesn't write)
            
            in_fd = fd[0];
            // Save read end of current pipe
            // This will be the input for the next command
            
            i++;
            // Move to next command
        }
    }
    
    while (wait(&status) > 0)
    // Wait for ALL child processes to finish
    // wait() returns -1 when no more children exist
    // &status captures the exit status of each child
    {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            // WIFEXITED: checks if child exited normally (not killed by signal)
            // WEXITSTATUS: extracts the exit code (0-255)
            // If child exited normally but with non-zero code (error)
            ret = 1;
            // Mark overall operation as failed
        else if (!WIFEXITED(status))
            // If child did NOT exit normally
            // (terminated by signal: SIGSEGV, SIGKILL, etc.)
            ret = 1;
            // Mark overall operation as failed
    }
    
    return ret;
    // Return 0 if all commands succeeded, 1 if any failed
}
