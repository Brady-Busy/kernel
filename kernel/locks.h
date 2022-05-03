#pragma once

typedef struct lock {
    int status; 
} lock_t;

void lock_init (lock_t * lock);

void lock_get (lock_t * lock);

void lock_release (lock_t * lock);