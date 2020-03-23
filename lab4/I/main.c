#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int i = 0;

int alarm_handler(void)
{
    alarm(1);
    i = 0;
    return 0;
}

int main (void)
{

    // Define new action
    struct sigaction new_action;
    new_action.sa_handler = alarm_handler;
    new_action.sa_flags = 0;

    sigaction(SIGALRM, &new_action, NULL);

    alarm(1);
    while(1)
    {
        printf("%d\n", i++);
    }

    return 0;
}