#ifndef _SCHED_H
#define _SCHED_H

// Amount of CPU time each process gets (in ms)
#define SCHED_QUANTUM	40

void schedule(void);

#endif
