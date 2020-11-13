#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdlib.h>

int main()
{
    pid_t th_pid = getpid();
    printf("PID = [%i]\n", th_pid);
    gid_t th_gid = getgid();
    printf("GID = [%i]\n", th_gid);
    th_gid = getegid();
    printf("Effective GID = [%i]\n", th_gid);

    mode_t old_mask = umask(0);
    printf("Umask = [%04x]\n", old_mask);
    umask(old_mask);

    struct passwd *pass = getpwuid(getuid());
    printf("User: '%s'\nPassword: '%s'\nUID: [%i]\nGID: [%i]\n", 
    pass->pw_name, pass->pw_passwd, pass->pw_uid, pass->pw_gid);

    char* workdir = get_current_dir_name();
    printf("CWD: '%s'\n", workdir);
    free(workdir);
    return 0;
}

#undef _GNU_SOURCE