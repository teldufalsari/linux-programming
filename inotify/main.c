/*
This program takes directory names 
and lists first 'number' files 
created in this directory
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <limits.h>
#include <linux/stat.h>
#include <time.h>
#include <signal.h>

#define __USE_ATFILE 1
#include <fcntl.h>
#undef __USE_ATFILE

#ifdef NAME_MAX
#define NAME_LEN NAME_MAX
#else
#define NAME_LEN 256
#endif

#define TIME_OUT_FORMAT "%a %Y-%m-%d %H:%M:%S"

char* g_buffer;
int HandleEvents(int dir_fd, int inot_fd, unsigned number_left);
ssize_t ReadEvent(int inot_fd, char* buffer, size_t* bufsize_p);
void term_handler(int sig)
{
    (void)sig;
    free(g_buffer);
    exit(0);
}

int main(int argc, char* argv[]) 
{
    if ((argc != 3) && (argc != 2)) {
        printf("Usage: %s directory [number of events]\n", argv[0]);
        return 1;
    }

    struct sigaction terminate_action = {};
    terminate_action.sa_flags = SA_RESTART;
    terminate_action.sa_handler = term_handler;
    sigemptyset(&terminate_action.sa_mask);
    if (sigaction(SIGINT, &terminate_action, NULL) == -1) {
        perror("sigaction");
        puts("Failed to specify termination behavior, program may work incorrectly");
    }

    int dir_fd = open(argv[1], __O_DIRECTORY | O_RDONLY);
    if (dir_fd < 0) {
        perror("Could not open directory");
        printf("%s\n", argv[1]);
        return 2;
    }
    int inot_fd = inotify_init();
    int watch_d = inotify_add_watch(inot_fd, argv[1], IN_CREATE);
    if (watch_d < 0) {
        perror("Could not open directory");
        printf("%s\n", argv[1]);
        return 2;
    }
    unsigned number_left = 0;
    if (argc == 3) {
        errno = 0;
        number_left = (unsigned)strtol(argv[2], NULL, 10);
        if (errno == ERANGE) {
            printf("%s is not a valid number, assuming zero instead\n", argv[2]);
            number_left = 0;
        }
    }
    int code = HandleEvents(dir_fd, inot_fd, number_left);
    close(inot_fd);
    close(dir_fd);
    return code;
}

int HandleEvents(int dir_fd, int inot_fd, unsigned number_left) {
    size_t buf_sz = sizeof(struct inotify_event) + NAME_LEN + 1;
    char* buffer = malloc(buf_sz);
    if (buffer == NULL) {
        perror("Memalloc error");
        return 3;
    }
    g_buffer = buffer;
    ssize_t read_size = 0;
    char* ptr;
    struct inotify_event* event;
    struct statx statbuf;
    time_t btime;
    struct tm timestruct;
    char timebuf[80];
    unsigned inc = 1;
    if (number_left == 0) {
        number_left = 1;  // Infinite loop
        inc = 0;
        }
    while (number_left > 0) {
        if ((read_size = ReadEvent(inot_fd, buffer, &buf_sz)) < 0) {
            perror("Memalloc error");
            free(buffer);
            return 3;
        }
        for (ptr = buffer; (ptr < buffer + read_size) && (number_left > 0); 
        ptr += sizeof(struct inotify_event) + event->len) {
            event = (struct inotify_event *) ptr;
            if (event->mask == IN_CREATE) {
                if (syscall(SYS_statx, dir_fd, event->name,
                AT_SYMLINK_NOFOLLOW, STATX_BTIME, &statbuf)) {
                    perror("Stat");
                    printf("File: %s", event->name);
                    free(buffer);
                    return 4;
                }
                btime = statbuf.stx_btime.tv_sec;
                timestruct = *localtime(&btime);
                strftime(timebuf, sizeof(timebuf), TIME_OUT_FORMAT, &timestruct);
                printf("[%s] File %s was created\n", timebuf, event->name);
            }
            number_left -= inc;
        }
    }
    free(buffer);
    return 0;
}

/*
    This functions guarantees that 
    there will be enough space for the 
    event to be read unless 
    there are no memory errors.
*/
ssize_t ReadEvent(int inot_fd, char* buffer, size_t* bufsize_p) {
    ssize_t read_size = read(inot_fd, buffer, *bufsize_p);
    if (read_size < 0)
        return read_size;
    while ((size_t)read_size == *bufsize_p) {
        (*bufsize_p) *= 2;
        buffer = realloc(buffer, *bufsize_p);
        if (buffer == NULL) {
            return -1;
        }
        read_size += read(inot_fd, buffer + ((*bufsize_p) >> 1), (*bufsize_p) >> 1);
    }
    return read_size;
}

#undef NAME_LEN
#undef TIME_OUT_FORMAT
