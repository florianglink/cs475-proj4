#include <xinu.h>

int32 deadCount = 0;

void rag_request(pid32 pid, lid32 lockid)
{
    int tmPid = pid + NLOCK;
    rag[tmPid][lockid] = 1;
}

void rag_alloc(pid32 pid, lid32 lockid)
{
    rag[lockid][pid + NLOCK] = 1;
    rag[pid + NLOCK][lockid] = 0;
}

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

int dfs(int32 deadIndx, int32 vertex, int32 *visited)
{
    int32 currHorizPid;
    int32 row;
    // Vertex = Current Testing Lock
    visited[vertex] = 1;
    // int deadIndex = 0;
    for (currHorizPid = 0; currHorizPid < (NLOCK + NPROC); currHorizPid++) // i = col (pid)
    {
        if (rag[vertex][currHorizPid])
        {
            for (row = NLOCK; row < (NLOCK + NPROC); row++) // Go through col of Lock vertex
            {
                if (rag[row][vertex] == 1 && row != currHorizPid) // If detecting a 1 other than currHorizPid
                {
                    deadCount++;
                    if (deadCount == 1)
                    {
                        kprintf("DEADLOCK DETECTED: ");
                    }
                    kprintf("pid = %d lockid = %d ", currHorizPid - NLOCK, vertex);
                }
            }
        }
    }
    if (vertex < NLOCK - 1)
    {
        deadIndx = dfs(deadIndx, vertex + 1, visited);
    }
    return deadIndx;
}

int deadlock_detect()
{
    int32 i;
    int32 visited[NLOCK + NPROC];
    for (i = 0; i < (NLOCK + NPROC); i++)
    {
        visited[i] = 0;
    }
    deadCount = 0;
    return dfs(0, 0, visited);
}

// TODO - add in your functions
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
