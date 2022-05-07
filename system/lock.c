#include <xinu.h>

local lid32 newlock(void);
pid32 currpid;

/**
 * Create a new semaphore and return the ID to the caller
 * @return ID of the mutex to caller, or SYSERR
 */
lid32 lock_create()
{
	intmask mask; /* saved interrupt mask	*/
	lid32 lockid; /* lock ID to return	*/

	mask = disable();

	if ((lockid = newlock()) == SYSERR)
	{
		restore(mask);
		return SYSERR;
	}

	restore(mask);
	return lockid;
}

/**
 * Allocate an unused lock and return its index
 * @return	ID of free lock
 */
local lid32 newlock(void)
{
	static lid32 nextlock = 0; /* next lockid to try	*/
	lid32 lockid;			   /* ID to return	*/
	int32 i;				   /* iterate through # entries	*/

	// TODO START

	// TODO - loop through each element in the lock table.
	for (i = 0; i < NLOCK; i++)
	{
		// TODO - and find a lock that is free to use
		// TODO - set its state to used, and reset the mutex to FALSE
		if (locktab[i].state = LOCK_FREE)
		{
			locktab[i].state = LOCK_USED;
			locktab[i].lock = FALSE;
			// TODO - return its lockid
			lockid = i;
			return lockid;
		}
	}

	// TODO - if there is no such lock, return SYSERR
	return SYSERR;

	// TODO END
}

/**
 * Delete a lock by releasing its table entry
 * @param ID of lock to release
 */
syscall lock_delete(lid32 lockid)
{
	intmask mask;			/* saved interrupt mask		*/
	struct lockentry *lptr; /* ptr to lock table entry	*/

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}

	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// TODO START

	// TODO - reset lock's state to free and the mutex to FALSE
	lptr->state = LOCK_FREE;
	lptr->lock = FALSE;

	// TODO - remove all processes waiting on its queue, and send them to the ready queue
	while (!isempty(lptr->wait_queue))
	{
		currpid = dequeue(lptr->wait_queue);
		enqueue(currpid, readyqueue, 0);
	}

	// TODO (RAG) - remove all RAG edges to and from this lock

	// TODO END

	resched();
	restore(mask);
	return OK;
}

/**
 * Acquires the given lock, or cause current process to wait until acquired
 * @param lockid	lock on which to wait
 * @return status
 */
syscall acquire(lid32 lockid)
{
	intmask mask;			// saved interrupt mask
	struct lockentry *lptr; // ptr to sempahore table entry

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}

	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// TODO START
	// TODO - enqueue the current process ID on the lock's wait queue
	enqueue(currpid, lptr->wait_queue, 0);
	// TODO (RAG) - add a request edge in the RAG
	// TODO END

	restore(mask); // reenable interrupts

	// TODO START
	// TODO - lock the mutex!
	lptr->lock = TRUE;
	// TODO END

	mask = disable(); // disable interrupts

	// TODO START
	// TODO (RAG) - we reache this point. Must've gotten the lock! Transform request edge to allocation edge
	// TODO END

	restore(mask); // reenable interrupts
	return OK;
}

/**
 * Unlock, releasing a process if one is waiting
 * @param lockid ID of lock to unlock
 * @return status
 */
syscall release(lid32 lockid)
{
	intmask mask;			/* saved interrupt mask		*/
	struct lockentry *lptr; /* ptr to lock table entry	*/

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}
	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// TODO START
	// TODO - remove current process' ID from the lock's queue
	remove(currpid, lptr->wait_queue);

	// TODO - unlock the mutex
	lptr->lock = FALSE;

	// TODO (RAG) - remove allocation edge from RAG
	// TODO END

	restore(mask);
	return OK;
}
