//TODO - add your deadlock function prototypes
void rag_request(pid32 pid, lid32 lockid);
void rag_alloc(pid32 pid, lid32 lockid);
void dealloc(pid32 pid, lid32 lockid);
int dfs(int32 deadIndx, int32 vertex, int32 *visited);
int deadlock_detect();
void rag_print();

//TODO - add an "extern" declaration for the RAG
extern int32 rag[NLOCK + NPROC][NLOCK + NPROC];
