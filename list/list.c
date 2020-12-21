#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


int list_dir(int recursive_dirfd, const char* dir_name);
int dot_or_dotdot(const char* name);
const char* get_type(mode_t if_type);
int indent = 0;
#define INDENT_DEPTH 4
int dummy = 0;
int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        printf("Usage: %s [directory]\n", argv[0]);
        return 0;
    }
    char* dir_name;
    if (argc == 1)
        dir_name = getcwd(NULL, 0);
    else
        dir_name = argv[1];
    int dir_fd = open(dir_name, __O_DIRECTORY);
    if (dir_fd < 0)
    {
        perror("Could not open given directory");
        return 1;
    }
    close(dir_fd);
    
    int code = list_dir(dir_fd, dir_name);
    if (argc == 1)
        free(dir_name);
    return code;
}

int list_dir(int recursive_dirfd, const char* dir_name)
{
    if (dot_or_dotdot(dir_name))
    return 0;
    int cur_dirfd = openat(recursive_dirfd, dir_name, __O_DIRECTORY);
    if (cur_dirfd < 0)
    {
        perror("Could not open directory");
        return 1;
    }
    DIR* dir_s = fdopendir(cur_dirfd);
    if (dir_s == NULL)
    {
        perror("Error during opening the directory");
        return 2;
    }
    
    struct dirent* cur_entry = NULL;
    struct stat entry_stat;
    errno = 0;
    while ((cur_entry = readdir(dir_s)) != NULL)
    {
        if (fstatat(cur_dirfd, cur_entry->d_name, &entry_stat, AT_SYMLINK_NOFOLLOW) == -1) {
            puts("???");
            continue;
        }
        
        printf(
            "%*s%s\t%li bytes\t%s\n",
            indent, "",
            get_type(entry_stat.st_mode),
            entry_stat.st_size,
            cur_entry->d_name
        );
        if ((entry_stat.st_mode & __S_IFMT) == __S_IFDIR)
        {
            indent += INDENT_DEPTH;
            if (list_dir(cur_dirfd, cur_entry->d_name) != 0)
                return 1;
            indent -= INDENT_DEPTH;
        }
        errno = 0;
    }
    if (errno != 0)
    {
        perror("Error during reding the directory");
        if (cur_entry)
            printf("%s\n", cur_entry->d_name);
        return 1;
    }
    closedir(dir_s);
    close(cur_dirfd);
    return 0;
}

int dot_or_dotdot(const char* name)
{
    if (name[0] == '.')
    {
        char sep = name[(name[1] == '.') + 1];
        return ((!sep) || (sep == '/'));
    }
    return 0;
}

const char* get_type(mode_t if_type)
{
    switch (if_type & __S_IFMT)
    {
    case __S_IFREG:
        return "regular";
    case __S_IFSOCK:
        return "socket";
    case __S_IFDIR:
        return "directory";
    case __S_IFIFO:
        return "fifo";
    case __S_IFLNK:
        return "link";
    case __S_IFBLK:
        return "block device";
    case __S_IFCHR:
        return "character device";
    default:
        return "???";
    }
}