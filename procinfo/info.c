#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <pwd.h>
#include <stdlib.h>
#include <errno.h>
#include <grp.h>

int main()
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

    mode_t old_mask = umask(0);
    printf("Umask = [%04x]\n", old_mask);
    umask(old_mask);

    char* workdir = get_current_dir_name();
    if (workdir == NULL) {
        puts("CWD: ???");
    } else {
    printf("CWD: '%s'\n", workdir);
    free(workdir);
    }

    errno = 0;
    int priority = getpriority(PRIO_PROCESS, th_pid);
    if ((priority == -1) && (errno != 0)) {
        perror("getpriority");
        return 4;
    }
    printf("Scheduling priority: [%d]\n", priority);

    int groups_list_size = getgroups(0, NULL);
    if (groups_list_size < 0) {
        perror("getgroups");
        return 3;
    }
    gid_t* groups_list = calloc((unsigned)groups_list_size, sizeof(gid_t));
    if (groups_list == NULL) {
        perror("Memory allocation error");
        return 1;
    }
    if (getgroups(groups_list_size, groups_list) == -1) {
        perror("getgroups");
        free(groups_list);
        return 2;
    }
    struct group* gr_info_p;
    printf("Supplementary groups:");
    for (unsigned i = 0; i < (unsigned)groups_list_size; i++) {
        printf(" [%d]", groups_list[i]);
        gr_info_p = getgrgid(groups_list[i]);
        if (gr_info_p == NULL)
            printf("-(?)");
        else
            printf("-(%s)", gr_info_p->gr_name);
    }
    putchar('\n');
    free(groups_list);

    struct passwd *pass = getpwuid(getuid());
    printf(
        "User: '%s'\nPassword: '%s'\nUID: [%i]\nGID: [%i]\n", 
        pass->pw_name, pass->pw_passwd, pass->pw_uid, pass->pw_gid
    );
    return 0;
}

#undef _GNU_SOURCE