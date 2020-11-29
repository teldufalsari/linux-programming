#include <mqueue.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULT_MSG_SIZE 8192
#define DEFAULT_MSG_NUM 16
typedef struct {
    mqd_t queue_des;
    const char* queue_name;
} queue_ident_for_exit;
queue_ident_for_exit g_exit_ident = {};

void handler(int sig) {
    (void)sig;
    if (mq_close(g_exit_ident.queue_des))
        perror("mq_close");
    if (mq_unlink(g_exit_ident.queue_name)) {
        perror("Could not remove message queue file");
        exit(1);
    }
    exit(0);
}

int main(int argc, char* argv[])
{
    long msg_size = DEFAULT_MSG_SIZE, msg_num = DEFAULT_MSG_NUM;
    int c;
    struct option long_options[] = {
        {"msgsize", required_argument, NULL, 1},
        {"msgcount", required_argument, NULL, 2}
    };
    while ((c = getopt_long_only(argc, argv, "", long_options, NULL)) != -1) {
        switch (c)
        {
        case 1: // max mesage size
            errno = 0;
            long requested_size = strtol(optarg, NULL, 10);
            if (errno != 0) {
                perror("Could not retrieve max message length value");
                printf("Setting default value: %li\n", msg_size);
            } else if (requested_size <= DEFAULT_MSG_SIZE) {
                printf("Too small message size '%li', setting default value: %li\n", requested_size, msg_size);
            } else {
                msg_size = requested_size;
            }
            break;

        case 2: // max number of messages
            errno = 0;
            long requested_num = strtol(optarg, NULL, 10);
            if (errno != 0) {
                perror("Could not retrieve max messages mumber");
                printf("Setting default value: %li\n", msg_num);
            } else if (requested_num <= 0){
                printf("Invalid message number '%li', setting default value: %li\n", requested_num, msg_num);
            } else {
                msg_num = requested_num;
            }
            break;
        
        default: // unreachable
            break;
        }
    }
    if ((argc - optind) != 1) {
        printf("Usage: %s [--msgsize x] [--msgcount y] /queue_name\n", argv[0]);
        return 1;
    }

    struct sigaction term_action = {};
    term_action.sa_flags = SA_RESTART;
    term_action.sa_handler = handler;
    if ((sigaction(SIGTERM, &term_action, NULL))
    || (sigaction(SIGINT, &term_action, NULL))) {
        perror("sigaction");
        return 1;
    }

    mqd_t queue = mq_open(
        argv[optind],
        O_RDONLY | O_CREAT | O_EXCL,
        0620,
        NULL
    );
    if (queue < 0) {
        perror("Could not open queue file");
        return 1;
    }
    struct mq_attr queue_attrs;
    if (mq_getattr(queue, &queue_attrs)) {
        perror("Could not read attributes");
        mq_close(queue);
        mq_unlink(argv[optind]);
        return 1;
    }
    queue_attrs.mq_msgsize = msg_size;
    queue_attrs.mq_maxmsg = msg_num;
    if (mq_setattr(queue, &queue_attrs, NULL)) {
        perror("Could not set queue attribues");
        mq_close(queue);

    }

    g_exit_ident.queue_des = queue;
    g_exit_ident.queue_name = argv[optind];

    char* buffer = (char*)malloc(msg_size + 1);
    ssize_t received = 0;
    unsigned priority = 0;
    while (1) {
        received = mq_receive(queue, buffer, msg_size, &priority);
        if (received < 0) {
            perror("Failed to receive a message");
            mq_close(queue);
            mq_unlink(argv[optind]);
            return 1;
        }
        printf("<%u> %.*s\n", priority, received, buffer);
    }
    return 0;
}
