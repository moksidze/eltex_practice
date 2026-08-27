#include "timer.h"

#include <signal.h>
#include <string.h>

int create_driver_timer(timer_t *timer_id)
{
    struct sigevent event;

    memset(&event, 0, sizeof(event));

    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGRTMIN;

    return timer_create(CLOCK_MONOTONIC, &event, timer_id);
}

int start_driver_timer(timer_t timer_id, int seconds)
{
    struct itimerspec timer_spec;

    memset(&timer_spec, 0, sizeof(timer_spec));

    timer_spec.it_value.tv_sec = seconds;

    return timer_settime(timer_id, 0, &timer_spec, NULL);
}

void stop_driver_timer(timer_t timer_id)
{
    struct itimerspec timer_spec;

    memset(&timer_spec, 0, sizeof(timer_spec));

    timer_settime(timer_id, 0, &timer_spec, NULL);
}

void delete_driver_timer(timer_t timer_id)
{
    timer_delete(timer_id);
}