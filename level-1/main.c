#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int	picoshell(char **cmds[])
{
	
}


int main(void)
{
    // Test 1: Simple command - ls
    char *cmd1[] = {"ls", "-l", NULL};
    char **test1[] = {cmd1, NULL};
    
    printf("=== Test 1: ls -l ===\n");
    int ret1 = picoshell(test1);
    printf("Return: %d\n\n", ret1);
    
    // Test 2: Pipeline - ls | grep c
    char *cmd2_1[] = {"ls", NULL};
    char *cmd2_2[] = {"grep", "c", NULL};
    char **test2[] = {cmd2_1, cmd2_2, NULL};
    
    printf("=== Test 2: ls | grep c ===\n");
    int ret2 = picoshell(test2);
    printf("Return: %d\n\n", ret2);
    
    // Test 3: Multiple pipes - ls | grep c | wc -l
    char *cmd3_1[] = {"ls", NULL};
    char *cmd3_2[] = {"grep", "c", NULL};
    char *cmd3_3[] = {"wc", "-l", NULL};
    char **test3[] = {cmd3_1, cmd3_2, cmd3_3, NULL};
    
    printf("=== Test 3: ls | grep c | wc -l ===\n");
    int ret3 = picoshell(test3);
    printf("Return: %d\n\n", ret3);
    
    // Test 4: Error case - comando inexistente
    char *cmd4[] = {"comando_que_no_existe", NULL};
    char **test4[] = {cmd4, NULL};
    
    printf("=== Test 4: comando_que_no_existe (error expected) ===\n");
    int ret4 = picoshell(test4);
    printf("Return: %d\n\n", ret4);
    
    return 0;
}
