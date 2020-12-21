#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main (int argc, char* argv[])
{
    if (argc != 3)
    {
        printf ("Usage: %s /destination_name \"message\"\n", argv[0]);
        return 1;
    }
    mqd_t dst = mq_open(argv[1], O_WRONLY);
    if (dst < 0) {
        perror("Could not open queue file");
        return 1;
    }
    if (mq_send(dst, argv[2], strlen(argv[2]), 2)) {
        perror("Could not send message");
        mq_close(dst);
        return 1;
    }
    mq_close(dst);
    return 0;
}