#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "graph.h"
#include "evolution.h"
#include "threadpool.h"
#include "myrand.h"

#define BEST_ACTIVITY_NOW 1
#define SLEEPIN 1
//TODO разобраться с рандомом
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
    int vert_amount = -1;
    if (!strcmp(argv[4], "--file") || !strcmp(argv[4], "-f")) {
        mygraph = graph_read_file(argv[5]);
    }
    if (!strcmp(argv[4], "--generate") || !strcmp(argv[4], "-f")) {
        mygraph = graph_generate(atoi(argv[5]), 42);
        vert_amount = atoi(argv[5]);
    }

    evolution evo;
    //printGraph(mygraph);
    printf("\n");
    evolution_init(&evo, mygraph, N, N/2, 0.05, pthreads);   //не забыть условие для доп потоков!

    myrand_settings rnd;
    myrand_init(&rnd, rand(), 100, 10);//S);
    int tmp_steps = 100;
    int step = 0;
    int result = -1;
    int stable_steps = 0;
    int whole_steps = 0;
    struct timespec mt1, mt2;

    FILE *file1;
    file1 = fopen("inter_info.txt", "a");
    if (file1 == NULL) {
        printf("Can't open file!\n");
        return 7;
    }


    clock_gettime(CLOCK_MONOTONIC, &mt1);       //как в clion, в makefile добавить openmp?!
                                                //(раньше без них писал, флаг fopenmp)
    while (BEST_ACTIVITY_NOW == SLEEPIN) {
        crossover(&evo);
        selection(&evo);
        mutation(&evo);
        ++whole_steps;

        printf("%d\n",minpath(&evo));
        if (result == minpath(&evo)){
            ++stable_steps;
            if (stable_steps == S)
                break;
        } else {
            stable_steps = 0;
            result = minpath(&evo);
        };

        fprintf(file1, "%d %d %d\n", evo.minimum, evo.maximum, evo.average);
    }
    fclose(file1);
    myrand_destroy(&rnd);
    endWork(&evo);
    clock_gettime(CLOCK_MONOTONIC, &mt2);
    FILE *file;
    file = fopen("stats.txt", "a");
    if (file == NULL) {
        printf("Can't open file!\n");
        return 8;
    }
    if (vert_amount != -1)
        fprintf(file, "%d %d %d %d %d %ld %d \n", pthreads, N, S, vert_amount, whole_steps,
                mt2.tv_sec - mt1.tv_sec, result);
    fclose(file);
    printf("Hello, world!\n");
    return 0;
}