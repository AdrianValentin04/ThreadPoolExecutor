/* Gheorghe Adrian-Valentin 331CB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functiiAux.h"

int main(int argc, char** argv)
{
    FILE* input = fopen(argv[1], "r");
    if (!input)
        return 0;

    FILE* output = fopen(argv[2], "w");
    if (!output)
        return 0;

    int cores = 0;
    TSTACK* threadPool = createS(sizeof(TTHREAD));
    TQUEUE* waiting = createQ(sizeof(TTASK));
    TQUEUE* running = createQ(sizeof(TTASK));
    TQUEUE* finished = createQ(sizeof(TTASK));

    TDATA* data = calloc(1, sizeof(*data));
    if (!data)
        return 0;

    char line[MAX_LINE_LEN] , *token;

    fgets(line, MAX_LINE_LEN, input);
    token = strtok(line, " \n\0");
    data->timeMax = atoi(token);

    fgets(line, MAX_LINE_LEN, input);
    token = strtok(line, " \n\0");
    cores = atoi(token);

    data->avThreads = cores * 2;
    data->nrThreads = data->avThreads;

    data->taskIds = calloc(MAX_TASK_COUNT, sizeof(int));
    if (!data->taskIds)
        return 0;

    TTHREAD thread;
    thread.id = data->avThreads;

    int i;
    for (i = data->avThreads; i > 0; i--) {
        thread.id --;
        Push(threadPool, &thread);
    }

    while (fgets(line, MAX_LINE_LEN, input))
    {
        token = strtok(line, " ");
        if (token == NULL)
            break;
        else if (!strcmp(token, "add_tasks")) {
        int noTasks;
        TTASK aux;

        token = strtok(NULL, " \n\0");
        noTasks = atoi(token);

        token = strtok(NULL, " \n\0");
        aux.execTime = atoi(token);
        aux.remaining_time = aux.execTime;
        
        token = strtok(NULL, " \n\0");
        aux.priority = atoi(token);

        if (data->avThreads)
            addTask(waiting, data, noTasks, aux, output);

        } else if (!strcmp(token, "get_task")) {

            token = strtok(NULL, " \n\0");
            short int taskIds = atoi(token);
            getTask( running, waiting, finished, data, taskIds, output);

        } else if (!strcmp(token, "get_thread")) {

            token = strtok(NULL, " \n\0");
            char id = atoi(token);
            getThread(running, data, id, output);

        } else if (!strcmp(token, "print")) {

            token = strtok(NULL, " \n\0");

            if (!strcmp(token, "running")) {

                fprintf(output, "====== Running in parallel =======\n[");
                printQ(running, printRunning, output);

            } else if (!strcmp(token, "waiting")) {

                fprintf(output, "====== Waiting queue =======\n[");
                printQ(waiting, printWaiting, output);

            } else if (!strcmp(token, "finished")) {

                fprintf(output, "====== Finished queue =======\n[");
                printQ(finished, printFinished, output);

            }
            fprintf(output, "]\n");

        } else if (!strcmp(token, "run")) {
            
            token = strtok(NULL, " \n\0");
            int time = atoi(token);

            fprintf(output, "Running tasks for %d ms...\n", time);
            run(running, finished, waiting, threadPool, data, time, output);

        } else if (!strcmp(token, "finish\n")) {
            
            int maxTime = auxRun(running, finished, waiting, threadPool, data, 
                                data->timeMax);
            
            // adaugam timpul returnat in timpul total de executie
            if (running->list->nrCells) {
                data->totalTime += data->timeMax;
            } else
                data->totalTime += maxTime;
            
            // rulam pana la final
            for (;running->list->nrCells; ) {
                maxTime = auxRun(running, finished, waiting, threadPool, data, 
                                data->timeMax);
                if (running->list->nrCells) {
                    data->totalTime += data->timeMax;
                } else
                    data->totalTime += maxTime;
            }

            fprintf(output, "Total time: %d", data->totalTime);
            break;
        }
        
    }

    freeAll(running, finished, waiting, threadPool, data);
    fclose(input);
    fclose(output);

    return 0;
}
