#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "graph.h"
#include "evolution.h"
#include "threadpool.h"
//ТАМ srand в начале стоит! не забыть!


int main(int argc, char *argv[]){
    srand(time(NULL));
    int pthreads;         //Не забыть условие для дополнительных потоков
    int N;                //начальный размер популяции
    int S;                //параметр для критерия остановки
    char* flag;           //--generate или --file

    graph_t* mygraph;

    pthreads = atoi(argv[1]);
    N = atoi(argv[2]);
    S = atoi(argv[3]);

    if (!strcmp(argv[4], "--file") || !strcmp(argv[4], "-f")) {
        mygraph = graph_read_file(argv[5]);
    }
    if (!strcmp(argv[4], "--generate") || !strcmp(argv[4], "-f")) {
        mygraph = graph_generate(atoi(argv[5]), 10);
    }
    evolution evo;
    printGraph(mygraph);
    printf("\n");
    evolution_init(&evo, mygraph, N, N/2, 0.5, pthreads);   //не забыть условие для доп потоков!

    mutation(&evo);
    printGraph(mygraph);
    crossover(&evo);
    selection(&evo);

    endWork(&evo);

    printf("Hello, world!\n");
    return 0;
}