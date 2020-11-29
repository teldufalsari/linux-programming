#include <stdio.h>
#include <mqueue.h>

int main()
{
    mqd_t default_queue = mq_open(
        "/def_queue",
        O_RDWR | O_CREAT | O_EXCL,
        0600,
        NULL
        );
    if (default_queue < 0) {
        perror("Could not open test queue");
        return 1;
    }
    struct mq_attr default_attrs;
    if (mq_getattr(default_queue, &default_attrs)) {
        perror("Could not read attributes");
        mq_close(default_queue);
        return 1;
    }
    printf(
        "O_NONBLOCK: %li\nMax number of messages: %li\nMax message size: %li\n",
        default_attrs.mq_flags,
        default_attrs.mq_maxmsg,
        default_attrs.mq_msgsize
    );
    mq_close(default_queue);
    if (mq_unlink("/def_queue")) {
        perror("Could not remove message queue file");
        return 1;
    }
    return 0;
}
