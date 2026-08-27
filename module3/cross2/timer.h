#ifndef TIMER_H
#define TIMER_H

#include <time.h>

int create_driver_timer(timer_t *timer_id);

int start_driver_timer(timer_t timer_id, int seconds);

void stop_driver_timer(timer_t timer_id);

void delete_driver_timer(timer_t timer_id);

#endif