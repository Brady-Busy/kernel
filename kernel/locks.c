#include "locks.h"

// Sourced from https://compas.cs.stonybrook.edu/~nhonarmand/courses/fa17/cse306/slides/11-locks.pdf
// Test and set method for atomicity when checking locks
int TAS(volatile int *addr, int newval) {
         int result = newval;
         asm volatile("lock; xchg %0, %1"
                     : "+m" (*addr), "=r" (result)
                     : "1" (newval)
                     : "cc");
         return result;
    }
void lock_init (lock_t * lock) {
    lock->status = 0; // 0 will mean available
}

void lock_get (lock_t * lock) {
    // loop until available
    // after once it gets it, it will immediately update lock status and return
    while (TAS(&lock->status, 1) == 1); 
}

void lock_release (lock_t * lock) {
    lock->status = 0;
}