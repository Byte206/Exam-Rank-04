#include "../../header.h"
// Includes the header file with necessary definitions

int picoshell(char **cmds[])
// Function that receives an array of arrays of strings (commands with their arguments)
// Example: cmds[0] = {"ls", "-l", NULL}, cmds[1] = {"grep", "txt", NULL}

{
    int i = 0;
    // Index to iterate over the commands in the cmds[] array
    
    int fd[2];
    // Array to store pipe descriptors: fd[0]=read, fd[1]=write
    
    int in_fd = 0;
    // Descriptor that stores the input of the current command
    // Initialized to 0 (stdin) for the first command
    
    pid_t pid;
    // Variable to store the child process ID after fork()
    
    if (!cmds)
        return 1;
    // If cmds is NULL, return 1 (error)
    
    while (cmds[i])
    // While there are commands to process (until NULL is found)
    {
        if (cmds[i + 1])
        // If there is a next command (not the last one)
            pipe(fd);
        // Creates a pipe: fd[0] for reading, fd[1] for writing
        else
        // If it's the last command
        {
            fd[0] = -1;
            // Marks fd[0] as invalid
            fd[1] = -1;
            // Marks fd[1] as invalid (doesn't need output pipe)
        }
        
        pid = fork();
        // Creates a child process. Returns 0 to child, and child's PID to parent
        
        if (pid == 0)
        // Code executed by the CHILD PROCESS
        {
            if (in_fd != 0)
            // If there is a redirected input (not standard stdin)
            {
                dup2(in_fd, STDIN_FILENO);
                // Duplicates in_fd onto stdin (descriptor 0)
                // Now stdin reads from the previous pipe
                
                close(in_fd);
                // Closes the original descriptor (no longer needed, it's duplicated)
            }
            
            if (fd[1] != -1)
            // If there is an output pipe (not the last command)
            {
                dup2(fd[1], STDOUT_FILENO);
                // Duplicates fd[1] onto stdout (descriptor 1)
                // Now stdout writes to the pipe towards the next command
                
                close(fd[1]);
                // Closes the original write descriptor
                
                close(fd[0]);
                // Closes the read descriptor (the child doesn't use it)
            }
            
            execvp(cmds[i][0], cmds[i]);
            // Replaces the current process with the command to execute
            // cmds[i][0] is the command name, cmds[i] are all its arguments
            // If execvp succeeds, it never returns (the process is replaced)
            
            exit(0);
            // Only executes if execvp fails. Terminates the child process
        }
        else
        // Code executed by the PARENT PROCESS
        {
            if (in_fd != 0)
                close(in_fd);
            // Closes the previous pipe (the parent no longer needs it)
            
            if (fd[1] != -1)
                close(fd[1]);
            // Closes the write end of the current pipe
            // The parent doesn't write to the pipe, only the child does
            
            in_fd = fd[0];
            // Saves the read end of the current pipe
            // This will be the input of the NEXT command
            
            i++;
            // Advances to the next command in the array
        }
    }
    
    while (wait(NULL) > 0)
         ;
    // Waits for ALL child processes to finish
    // wait() returns -1 when there are no more children
    // NULL indicates we don't care about the exit status
    
    return 0;
    // Returns 0 indicating success
}
