#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const av[], int type)
{

}


int main(void)
{
    char *args[] = {"ls", "-l", NULL};
    int fd = ft_popen("ls", args, 'r');
    if (fd < 0)
    {
        perror("ft_popen");
        return 1;
    }

    char buf[1024];
    ssize_t n;

    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0)
    {
        buf[n] = '\0';
        printf("%s", buf);
    }

    close(fd);
    return 0;
}

