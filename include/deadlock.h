//TODO - add your deadlock function prototypes
void rag_request(pid32 pid, lid32 lockid);
void rag_alloc(pid32 pid, lid32 lockid);
void dealloc(pid32 pid, lid32 lockid);
void dfs(int32 vertex, int32 *visited);
void  deadlock_detect();
void deadlock_recover();
void rag_print();

//TODO - add an "extern" declaration for the RAG
extern int32 rag[NLOCK + NPROC][NLOCK + NPROC];

//make a variable holding a deadlocked lock and victim
extern lid32 dlLock;
extern pid32 victim;
