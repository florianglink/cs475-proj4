/* resched.c - resched */

#include <xinu.h>

int32 counter = 0;
pid32 victim;
lid32 dlLock;

/**
 * Reschedule processor to next ready process
 *
 */
void resched(void) // assumes interrupts are disabled
{
	pid32 oldpid = currpid;
	struct procent *ptold; // ptr to table entry for old process
	struct procent *ptnew; // ptr to table entry for new process

	// If rescheduling is deferred, record attempt and return
	if (Defer.ndefers > 0)
	{
		Defer.attempt = TRUE;
		return;
	}

	// Point to process table entry for the current (old) process
	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR)
	{
		// Old process got preempted; place back on ready queue
		ptold->prstate = PR_READY;
		enqueue(currpid, readyqueue, ptold->prprio);
	}

	// Force context switch to next ready process
	currpid = dequeue(readyqueue);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;

	// DC: Aging
	if (AGING)
		sched_age(readyqueue, oldpid, currpid);

	// TODO
	preempt = QUANTUM;

	/* Call Deadlock Detect on 100 runs sometimes doesn't detect on 50 */
	counter++;
	if (counter == 200)
	{
		intmask mask = disable(); // disable interrupts
		//detect a deadlock
		deadlock_detect();
		counter = 0;
		//if there is a victim and its lock then recover from it
		if(victim != NULL && dlLock != NULL) {
			kprintf("\n");
			deadlock_recover();
		}
		kprintf("\n");
		// other code with interrupt disabled
		restore(mask); // reenable interrupts
	}

	// Context switch to next ready process
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	// Old process returns here when resumed

	return;
}

/**
 * Increment priority of each process on queue by one
 * @param q	queue of PCBs
 * @param oldpid	the old pid (switched out)
 * @param newpid	the new pid (switched in)
 */
void sched_age(struct queue *q, pid32 oldpid, pid32 newpid)
{
	if (nonempty(q))
	{
		struct qentry *curr = q->head;
		while (curr != NULL)
		{
			// do not increment priority of null proc, old proc, or new proc
			if (curr->pid != 0 && curr->pid != oldpid && curr->pid != newpid)
			{
				curr->key++;
				proctab[curr->pid].prprio++;
			}
			curr = curr->next;
		}
	}
}
