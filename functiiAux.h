/* Gheorghe Adrian-Valentin 331CB */

#include <stdlib.h>

#define MAX_LINE_LEN 10000
#define MAX_TASK_COUNT 32767

typedef struct GCell
{
    void *info;
    struct GCell *next;
} TGCell;

typedef struct {
    TGCell* firstCell;
    int nrCells;
    size_t sizeData;
} TLG;

typedef struct {
    TLG* list;
} TSTACK;

typedef struct {
    TLG* list;
} TQUEUE;

typedef struct {
    signed char id;
} TTHREAD;

typedef struct {
    int id;
    TTHREAD thread;
    signed char priority;
    int execTime;
    int remaining_time;
} TTASK;

typedef struct {
    int* taskIds;
    int timeMax;
    int avThreads;
    int totalTime;
    int nrThreads;
} TDATA;

/* ============================== Lists =========================== */

TLG* createList(size_t data_size);

void addFirst(TLG* list, void* data);

void addNode(TLG* list, int n, void* data);

void removeFirst(TLG* list);

void freeList(TLG** list);

/* ============================== Stack =========================== */

TSTACK* createS(size_t data_size);

void* Pop(TSTACK* s);

void Push(TSTACK* s, void* src);

void freeS(TSTACK** s);

/* ============================== Queue =========================== */

TQUEUE* createQ(size_t data_size);

void* deQ(TQUEUE* q);

void enQ(TQUEUE* q, int (*cmp_func)(void*, void*), void* data);

void enQFinal(TQUEUE* q, void* data);

void freeQ(TQUEUE** q);

void printQ(TQUEUE* q, void (*print_func)(void*, FILE*), FILE* output);

/* ============================== Checker =========================== */

void addTask(TQUEUE* waiting, TDATA* auxData, int noTasks, TTASK tmp_task, 
            FILE* output);

void getTask(TQUEUE* running, TQUEUE* waiting, TQUEUE* finished, TDATA* auxData,
            int taskIds, FILE* output);

void getThread(TQUEUE* running, TDATA* auxData, int id, FILE* output);

void printWaiting(void* data, FILE* output);

void printRunning(void* data, FILE* output);

void printFinished(void* data, FILE* output);

void run(TQUEUE* running, TQUEUE* finished, TQUEUE* waiting, TSTACK* threadPool, 
        TDATA* auxData, int time, FILE* output);

int minimum(int e1, int e2);

int cmpPriority(void* el1, void* el2);

int cmpExec(void* el1, void* el2);

int cmpId(void* el1, void* el2);

int cmpTasks(void* t1, void* t2);

void moveToRunning(TQUEUE* running, TQUEUE* waiting, TSTACK* threadPool);

int auxRun(TQUEUE* running, TQUEUE* finished, TQUEUE* waiting, 
            TSTACK* threadPool, TDATA* auxData, int time);

int verifyTask(TTASK *task, int id);

int verifyThread(TTASK *task, int id);

int searchTask(TQUEUE* q, int id);

int searchThread(TQUEUE* q, int id);

void freeAll(TQUEUE* running, TQUEUE* finished, TQUEUE* waiting, 
            TSTACK* threadPool, TDATA* data);
