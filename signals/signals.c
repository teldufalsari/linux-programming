#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pwd.h>


void handler(int sig);
char* swear_words[] = {
    "Dear %s, you underestimate my power!\n",
    "I have no permission to obey YOU, %s!\n",
    "Negative!\n",
    "Pathetic.\n",
    "Afraid of WinLock? What about LinLock, %s?\n"
    };

char* uname = NULL;

int main(void)
{
    struct passwd*  pass = getpwuid(getuid());
    uname = pass->pw_name;
    srand((unsigned)getpid());
    struct sigaction universal_sa;
    sigemptyset(&universal_sa.sa_mask);
    universal_sa.sa_sigaction = handler;
    for (int sig_num = 1; sig_num <= 31; sig_num++)
        sigaction(sig_num, &universal_sa, NULL);
    while (1)
    {
        pause();
    }
    return 0;
}

void handler(int sig)
{
    (void)sig;
    unsigned index = (unsigned)rand() % (sizeof(swear_words) / sizeof(char*));
    putchar('\n');
    printf(swear_words[index], uname);
}