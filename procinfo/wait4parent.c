#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>

int print_info(void);

int main()
{
    pid_t barn_pid = fork();
    switch (barn_pid)
    {
    case -1:
        perror("fork:");
        exit(1);

    case 0:
        printf("This is child\n");
        print_info();

        int parent_fd = (int)syscall(__NR_pidfd_open, getppid(), 0);
        if (parent_fd < 0)
        {
            perror("pidfd_getfd");
            exit(2);
        }
        struct pollfd pollstruct = {parent_fd, POLLIN, 0};
        printf("Waiting for parent's termination...\n");
        int ready = poll(&pollstruct, 1, -1);
        if (ready < 0)
        {
            perror("poll");
            exit(2);
        }
        print_info();
        break;

    default:
        printf("This is parent, PID = %i\n", getpid());
        sleep(3);
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
    if (workdir == NULL) {
        puts("CWD: ???");
    } else {
    printf("CWD: '%s'\n", workdir);
    free(workdir);
    }

    return 0;
}