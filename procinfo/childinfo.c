#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/wait.h>

int print_info(void);

int main()
{
    pid_t barn_pid = fork();

    switch (barn_pid)
    {
    case -1:
        perror("fork");
        return -1;

    case 0: //child process
        printf("This is child\n");
        print_info();
        break;
    default:
        printf("\nThis is parent\n");
        int status = 0;
        waitpid(barn_pid, &status, WUNTRACED);
        if (WIFEXITED(status)) {
            printf("Child exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("Child terminated by signal %i\n", WTERMSIG(status));
            psignal(WTERMSIG(status), NULL);
        }
    }
    return 0;
}

int print_info()
{
    pid_t th_pid = getpid();
    pid_t th_ppid = getppid();
    gid_t th_gid = getgid();
    gid_t th_egid = getegid();
    pid_t th_sid = getsid(th_pid);
    printf(
        "PID = [%d], Parent PID = [%d], GID = [%d], EGID = [%d], SID = [%d]\n",
        th_pid, th_ppid, th_gid, th_egid, th_sid
    );

    char* workdir = get_current_dir_name();
    printf("CWD: '%s'\n", workdir);
    free(workdir);

    return 0;
}

#undef _GNU_SOURCE