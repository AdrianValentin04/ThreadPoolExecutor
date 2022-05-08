/* Gheorghe Adrian-Valentin 331CB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functiiAux.h"

/* ============================= Lists ============================= */

TLG* createList(size_t sizeData)
{
    TLG *list = calloc(sizeof(TLG), 1);
    if (!list)
        return NULL;
    
    list->sizeData = sizeData;

    return list;
}

void addFirst(TLG* list, void* data)
{   
    TGCell *aux = calloc(1, sizeof(TGCell));
    if (!aux)
        return;

    aux->info = calloc(1, list->sizeData);
    if (!aux->info) {
        free(aux);
        return;
    }

    aux->next = list->firstCell;
    list->firstCell = aux;

    list->nrCells ++;
    memcpy(aux->info, data, list->sizeData);
}

void addNode(TLG* list, int n, void* data)
{
    TGCell* new_node = calloc(1, sizeof(TGCell));
    if (!new_node) 
        return;
    
    new_node->info = calloc(1, list->sizeData);
    if (!new_node->info)
        return;

    TGCell *current = list->firstCell;
    for (; n > 1; current = current->next, n--);

    if (current) 
        new_node->next = current->next;

    current->next = new_node;

    list->nrCells++;
    memcpy(new_node->info, data, list->sizeData);
}

void removeFirst(TLG *list)
{
    TGCell *aux = list->firstCell;
    list->firstCell = aux->next;
    list->nrCells --;

    free(aux->info);
    aux->info = NULL;
    free(aux);
}

void freeList(TLG** list)
{
    int i;
    int crt = (*list)->nrCells;
    for (i = 0; i < crt; i++) {
        removeFirst(*list);
    }

    free(*list);
    *list = NULL;
}

/* ============================= Stack ============================= */

TSTACK* createS(size_t sizeData)
{
    TSTACK* s = calloc(1, sizeof(TSTACK));
    if (!s)
        return NULL;

    s->list = createList(sizeData);
    return s;
}

void* Pop(TSTACK* s)
{
    void* aux = calloc(s->list->sizeData, sizeof(char));
    if (!aux)
        return NULL;

    memcpy(aux, s->list->firstCell->info, s->list->sizeData);
    removeFirst(s->list);
    return aux;
}

void Push(TSTACK* s, void* src)
{
    addFirst(s->list, src);
}

void freeS(TSTACK** s)
{
    freeList(&((*s)->list));
    free(*s);
}

/* ============================= Queue ============================= */

TQUEUE* createQ(size_t sizeData)
{
    TQUEUE* q = calloc(1, sizeof(*q));
    if (!q)
        return NULL;

    q->list = createList(sizeData);
    return q;
}

void* deQ(TQUEUE* q)
{
    void* aux = calloc(1, q->list->sizeData);
    if (!aux)
        return NULL;

    memcpy(aux, q->list->firstCell->info, q->list->sizeData);
    removeFirst(q->list);
    return aux;
}

void enQ(TQUEUE* q, int (*cmp_func)(void*, void*), void* data)
{    
    if (q->list->nrCells) {
        TGCell *current = q->list->firstCell;
        int index = 0;

        while (index < q->list->nrCells) {
            if (cmp_func(data, current->info) < 0) {
                if (index == 0) {
                    addFirst(q->list, data);
                } else {
                    addNode(q->list, index, data);
                }
                return;
            }
            index ++;
            current = current->next;
        }

        enQFinal(q, data);
    } else {
        addFirst(q->list, data);
    }
}

void enQFinal(TQUEUE* q, void* data)
{
    if (!q->list->nrCells)
        addFirst(q->list, data);
    else
        addNode(q->list, q->list->nrCells, data);
}

void freeQ(TQUEUE** q)
{
    freeList(&((*q)->list));
    free(*q);
}

void printQ(TQUEUE* q, void (*print_func)(void*, FILE*), FILE* output)
{
    int index = 0;
    while (index < q->list->nrCells) {
        void *data = deQ(q);
        print_func(data, output);

        enQFinal(q, data);
        free(data);

        if (index != q->list->nrCells - 1)
            fprintf(output, ",\n");
        
        index++;
    }
}

/* ============================= Checker ============================= */

// functii de comparare pentru taskuri
int cmpPriority(void* el1, void* el2) {
    if (((TTASK*)el1)->priority > ((TTASK*)el2)->priority)
        return -1;
    else if (((TTASK*)el1)->priority < ((TTASK*)el2)->priority)
        return 1;
    
    return 0;
}

int cmpExec(void* el1, void* el2) {
    if (((TTASK*)el1)->execTime > ((TTASK*)el2)->execTime)
        return 1;
    else if (((TTASK*)el1)->execTime < ((TTASK*)el2)->execTime)
        return -1;

    return 0;
}

int cmpId(void* el1, void* el2) {
    if (((TTASK*)el1)->id > ((TTASK*)el2)->id)
        return 1;
    else if (((TTASK*)el1)->id < ((TTASK*)el2)->id)
        return -1;
    
    return 0;
}

int cmpTasks(void* el1, void* el2)
{
    if (cmpPriority(el1, el2) == 0 && cmpExec(el1, el2) == 0 && 
        cmpId(el1, el2) == 0) {
        return 0;
    } else if (cmpPriority(el1,el2) != 0) {
        return cmpPriority(el1,el2);
    } else if (cmpExec(el1, el2) != 0) {
        return cmpExec(el1, el2);
    } else return cmpId(el1, el2);

    return 0;
}

void addTask(TQUEUE* waiting, TDATA* auxData, int noTasks, TTASK auxTask, 
            FILE* output)
{
    
    // adaugam taskuri in waiting si ii dam un id
    for (; noTasks > 0; noTasks--) {
        int index = 0;
        while (index < MAX_TASK_COUNT && auxData->taskIds[index]) {
            index ++;
        }

        auxData->taskIds[index] = 1;
        auxTask.id = index + 1;

        fprintf(output, "Task created successfully : ID %d.\n", auxTask.id);
        // adaugam in coada de asteptare
        enQ(waiting, cmpTasks, &auxTask);
    }
}

// funtii de afisare
void printWaiting(void* data, FILE* output)
{
    fprintf(output, "(%d: priority = %d, remaining_time = %d)",
                ((TTASK*)data)->id, 
                ((TTASK*)data)->priority,
                ((TTASK*)data)->remaining_time);
}

void printRunning(void* data, FILE* output)
{
    fprintf(output,
            "(%d: priority = %d, remaining_time = %d, running_thread = %d)",
            ((TTASK*)data)->id, 
            ((TTASK*)data)->priority,
            ((TTASK*)data)->remaining_time, 
            ((TTASK*)data)->thread.id);
}

void printFinished(void* data, FILE* output)
{
    fprintf(output, "(%d: priority = %d, executed_time = %d)",
                ((TTASK*)data)->id, 
                ((TTASK*)data)->priority,
                ((TTASK*)data)->execTime);
}

// adaugam in coada running
void moveToRunning(TQUEUE* running, TQUEUE* waiting, TSTACK* threadPool)
{
    void* auxThread = Pop(threadPool);
    void* auxTask = deQ(waiting);

    ((TTASK*)auxTask)->thread.id = ((TTHREAD*)auxThread)->id;
    enQ(running, cmpTasks, auxTask);

    free(auxTask);
    free(auxThread);
}

// functie ce simuleaza operatia run
int auxRun(TQUEUE* running, TQUEUE* finished, TQUEUE* waiting, 
            TSTACK* threadPool, TDATA* auxData, int time)
{
    for (; auxData->avThreads > 0; auxData->avThreads--) {
            if (!waiting->list->nrCells)
                break;
            else {
                moveToRunning(running, waiting, threadPool);
            }   
    }

    if (!running->list->nrCells)
        return 0;
    else {
        int max_remaining_time = 0;
        int i;
        for (i = 0; i < running->list->nrCells; i++) {
            void* tmp = deQ(running);

            ((TTASK*)tmp)->remaining_time -= time;
            
            if (((TTASK*)tmp)->remaining_time > 0) {
                enQFinal(running, tmp);
            } else {
                // daca un task se termina
                i--;
                auxData->avThreads++;

                // calculam timpul maxim din timpul rularii
                if ((((TTASK*)tmp)->remaining_time + time) > max_remaining_time)
                    max_remaining_time = time + ((TTASK*)tmp)->remaining_time;

                Push(threadPool, &(((TTASK*)tmp)->thread.id));
                auxData->taskIds[((TTASK*)tmp)->id - 1] = 0;
                
                enQFinal(finished, tmp); 
            }
            free(tmp);
        }

        return max_remaining_time;
    }
}

int minimum(int e1, int e2)
{
    return (e1 < e2) ? e1 : e2;
}

// rulam taskurile pentru o durata specificata in input
void run(TQUEUE* running, TQUEUE* finished, TQUEUE* waiting, TSTACK* threadPool, 
        TDATA* auxData, int time, FILE* output)
{
    int ok = 1;
    int maxTime = 0;
    while (ok) {
        maxTime = auxRun(running, finished, waiting, threadPool, auxData, 
                        minimum(auxData->timeMax, time));
        if (running->list->nrCells) {
            auxData->totalTime += minimum(auxData->timeMax, time);
        } else
            auxData->totalTime += maxTime;
        
        if (time < auxData->timeMax) {
            ok = 0;
        }
        time -= auxData->timeMax;
    }
}

void getTask( TQUEUE* running, TQUEUE* waiting, TQUEUE* finished, TDATA* auxData, 
            int taskIds, FILE* output)
{
    int nrQ = 0;

    // cautam in ce coada se afla taskul
    int index = searchTask(running, taskIds);
    if (index >= 0)
        nrQ = 1;
    else {
        index = searchTask(waiting, taskIds);
        if (index >= 0)
            nrQ = 2;
        else {
            index = searchTask(finished, taskIds);
            if (index >= 0)
                nrQ = 3;
        }
    }

    TTASK *elem = NULL;
    void *aux = NULL;
    
    // obtinem informatiile taskului cerut
    if (index >= 0) {
        int i, j;
        switch (nrQ)
        {
        case 1:
            // scoatem toate elementele pana la cel cautat
            for (i = 0; i < index; i++) {
                aux = deQ(running);
                enQFinal(running, aux);
                free(aux);
            }

            // scoatem elementul cautat
            aux = deQ(running);
            elem = calloc(1, sizeof(TTASK));
            if (!elem) return;
            
            memcpy(elem, aux, sizeof(TTASK));
            
            enQFinal(running, aux);
            free(aux);

            // luam si restul elementelor si le punem inapoi in coada
            // ca sa pastram ordinea
            for (j = index + 1; j < running->list->nrCells; j++) {
                aux = deQ(running);
                enQFinal(running, aux);
                free(aux);
            }
            
            fprintf(output, "Task %d is running (remaining_time = %d).\n",
                    elem->id, elem->remaining_time);
            free(elem);
            return;
        // pentru celelalte cazuri am procedat la fel
        case 2:
            for (i = 0; i < index; i++) {
                aux = deQ(waiting);
                enQFinal(waiting, aux);
                free(aux);
            }

            aux = deQ(waiting);
            elem = calloc(1, sizeof(TTASK));
            if (!elem) return;
            
            memcpy(elem, aux, sizeof(TTASK));
            
            enQFinal(waiting, aux);
            free(aux);

            for (j = index + 1; j < waiting->list->nrCells; j++) {
                aux = deQ(waiting);
                enQFinal(waiting, aux);
                free(aux);
            }

            fprintf(output, "Task %d is waiting (remaining_time = %d).\n",
                    elem->id, elem->remaining_time);

            free(elem);
            return;

        case 3:
            for (i = 0; i < index; i++) {
                aux = deQ(finished);
                enQFinal(finished, aux);
                free(aux);
            }

            aux = deQ(finished);
            elem = calloc(1, sizeof(TTASK));
            if (!elem) return;
            
            memcpy(elem, aux, sizeof(TTASK));
            
            enQFinal(finished, aux);
            free(aux);

            for (j = index + 1; j < finished->list->nrCells; j++) {
                aux = deQ(finished);
                enQFinal(finished, aux);
                free(aux);
            }

            fprintf(output, "Task %d is finished (executed_time = %d).\n",
                    elem->id, elem->execTime);

            free(elem);
            return;
        }
    } else {
        fprintf(output, "Task %d not found.\n", taskIds);
        free(elem);
    }
}

// functii de verificare a egalitatii dupa id
int verifyTask(TTASK *task, int id)
{
    return ((TTASK*)task)->id == id;
}

int verifyThread(TTASK *task, int id)
{
    return ((TTASK*)task)->thread.id == id;
}

// returneaza index ul unde se gaseste taskul
int searchTask(TQUEUE* q, int id)
{
    int index = -1;

    int i, j;
    for (i = 0; i < q->list->nrCells; i++)
    {
        void *aux = deQ(q);

        if (verifyTask(aux, id)){
            index = i;
            enQFinal(q, aux);
            free(aux);

            for (j = i + 1; j < q->list->nrCells; j++) {
                aux = deQ(q);
                enQFinal(q, aux);
                free(aux);
            }
            break;
        }
        enQFinal(q, aux);
        free(aux);
    }

    return index;
}

int searchThread(TQUEUE* q, int id)
{
    int index = -1;

    int i, j;
    for (i = 0; i < q->list->nrCells; i++)
    {
        void *aux = deQ(q);

        if (verifyThread(aux, id)){
            index = i;
            enQFinal(q, aux);
            free(aux);

            for (j = i + 1; j < q->list->nrCells; j++) {
                aux = deQ(q);
                enQFinal(q, aux);
                free(aux);
            }
            break;
        }
        enQFinal(q, aux);
        free(aux);
    }

    return index;
}

// functie ce afiseaza informatiile despre un thread
void getThread(TQUEUE* running, TDATA* auxData, int id, FILE* output)
{
    if (id >= auxData->nrThreads)
        return;

    TTASK *elem = NULL;
    void *aux = NULL;
    int index = searchThread(running, id);

    // foloseste acelasi procedeu ca si la cautarea de taskuri
    if (index >= 0) {
        int i, j;
        for (i = 0; i < index; i++) {
            aux = deQ(running);
            enQFinal(running, aux);
            free(aux);
        }

        aux = deQ(running);
        elem = calloc(1, sizeof(TTASK));
        if (!elem) return;
        
        memcpy(elem, aux, sizeof(TTASK));
        
        enQFinal(running, aux);
        free(aux);

        for (j = index + 1; j < running->list->nrCells; j++) {
            aux = deQ(running);
            enQFinal(running, aux);
            free(aux);
        }

        fprintf(output,
            "Thread %d is running task %d (remaining_time = %d).\n",
            id, elem->id, elem->remaining_time);

        free(elem);
    } else {
        if (!elem) {
            fprintf(output, "Thread %d is idle.\n", id);
            return;
        }
    }
}

// functie de eliberare a memoriei
void freeAll(TQUEUE* running, TQUEUE* finished, TQUEUE* waiting, 
            TSTACK* threadPool, TDATA* data)
{
    free(data->taskIds);
    free(data);
    freeQ(&running);
    freeQ(&waiting);
    freeQ(&finished);
    freeS(&threadPool);
}
