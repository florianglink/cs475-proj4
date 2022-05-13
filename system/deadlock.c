#include <xinu.h>

int32 deadCount = 0;
lid32 dlLock;
pid32 victim;

/**
 * Create a request edge between the process id and lock id.
 * 
 * @param pid process id for request edge
 * @param lockid lockid for request edge
 */
void rag_request(pid32 pid, lid32 lockid)
{
    //process -> lock
    int tmPid = pid + NLOCK;
    rag[tmPid][lockid] = 1;
}

/**
 * Create an allocation edge between the process id and lock id.
 * 
 * @param pid process id for alloc edge
 * @param lockid lock id for alloc edge
 */
void rag_alloc(pid32 pid, lid32 lockid)
{
    //lock -> process
    rag[lockid][pid + NLOCK] = 1;
    rag[pid + NLOCK][lockid] = 0;
}

/**
 * Deallocate the pid from the locks whether it is a request edge or 
 * a alloc edge.
 * 
 * @param pid process id to deallocate
 * @param lockid lock id to deallocate edges from
 */
void dealloc(pid32 pid, lid32 lockid)
{
    // checks for a allocation edge
    if (rag[lockid][pid + NLOCK] == 1)
    {
        rag[lockid][pid + NLOCK] = 0;
    }
    // checks for a request edge
    else if (rag[pid + NLOCK][lockid] == 1)
    {
        rag[pid + NLOCK][lockid] = 0;
    }
}

/**
 * DFS algorithm to detect if there are any cycles within the RAG.
 * 
 * @param vertex the starting point for the DFS
 * @param visited array of visited elements
 */
void dfs(int32 vertex, int32 *visited)
{
    int32 currHorizPid;
    int32 row;
    // Vertex = Current Testing Lock
    visited[vertex] = 1;
    for (currHorizPid = 0; currHorizPid < (NLOCK + NPROC); currHorizPid++) // i = col (pid)
    {
        if (rag[vertex][currHorizPid])
        {
            for (row = NLOCK; row < (NLOCK + NPROC); row++) // Go through col of Lock vertex
            {
                if (rag[row][vertex] == 1 && row != currHorizPid) // If detecting a 1 other than currHorizPid
                {
                    deadCount++;
                    //print output
                    if (deadCount == 1)
                    {
                        kprintf("DEADLOCK DETECTED: ");
                    }
                    victim = currHorizPid - NLOCK;
                    dlLock = vertex;
                    kprintf("pid = %d lockid = %d ", currHorizPid - NLOCK, vertex);
                }
            }
        }
    }
    //keep checking vertexs
    if (vertex < NLOCK - 1)
    {
        dfs(vertex + 1, visited);
    }
}

/**
 * Detects a Deadlock calls to the dfs function and initializes the 
 * visited array.
 * 
 */
void deadlock_detect()
{
    int32 i;
    int32 visited[NLOCK + NPROC];
    for (i = 0; i < (NLOCK + NPROC); i++)
    {
        visited[i] = 0;
    }
    victim = NULL;
    dlLock = NULL;
    deadCount = 0;
    dfs(0, visited);
}

/**
 * Recovery from a deadlock
 * 
 */
void deadlock_recover()
{
    //initialize variables such as a lock and process pointer
    struct lockentry *lptr;
    struct lockentry *check; //hold lock for removing victim from processes
    struct procent *prptr;
    pid32 dequeuedProc;
    lptr = &locktab[dlLock];
    //dequeue all processes and put them on the wait queue
    while(nonempty(lptr->wait_queue))
    {
        dequeuedProc = dequeue(lptr->wait_queue);
        prptr = &proctab[dequeuedProc];
        prptr->prstate = PR_READY;
        enqueue(dequeuedProc, readyqueue, prptr->prprio);
        //check if process is holding the lock
        if(dequeuedProc == victim) {
            kprintf("Entered IF\n");
            kill(victim);
            //remove process from all lock queues if requested
            for(int32 lock = 0; lock < NLOCK; lock++) 
            {
                if(rag[victim + NLOCK][lock]) 
                {
                    check = &locktab[lock];
                    remove(dequeuedProc, check->wait_queue);
                }
            }
            kprintf("Removed\n");
            //mutex unlock the dlLock
            mutex_unlock(&lptr->lock);
            kprintf("Unloicked\n");
            //deallocate the victims allocation and request edges
            for(int32 lock = 0; lock < NLOCK; lock++) 
            {
                dealloc(victim, lock);
            }
        }
    }
    kprintf("DEADLOCK RECOVERED: Killing pid = %d to release lock = %d", victim, dlLock);
    /* TODO Figure out how to kill the process */
}

/**
 * Print the RAG matrix
 * 
 */
void rag_print()
{
    for (int i = 0; i < (NLOCK + NPROC); i++)
    {
        if (i < NLOCK)
        {
            kprintf("L%d ", i);
        }
        else
        {
            kprintf("P%d ", i - NLOCK);
        }
        for (int j = 0; j < (NLOCK + NPROC); j++)
        {
            kprintf("%d ", rag[i][j]);
        }
        kprintf("\n");
    }
}
