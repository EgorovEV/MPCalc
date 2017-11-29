//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <zconf.h>
#include "evolution.h"
#include "myrand.h"
#include "threadpool.h"

void swap_mutation(void*);

//граф подается на вход уже заполненным
void evolution_init(evolution* evo, graph_t* graph, const int population_size, const int old_survivals, const float mutations, int threads){
    evo->essence_len = graph->n;
    evo->best_essences = old_survivals;
    evo->mutation_factor = mutations;
    evo->essences_amount = population_size;
    evo->rouds = graph;
    evo->population = (int*)malloc(population_size * graph->n * sizeof(int));
    //TODO ТЕСТОВАЯ ВЫБОРКА!!!
    //ПОМЕНЯТЬ!
    for (int i =0; i< evo->essences_amount * evo->essence_len; ++i){
        evo->population[i] = i;
    }
    evo->threadpool = threadpool_create(threads, 100);
}

void selection(evolution* evo){
    printf("Selection!\n");
}

void mutation(evolution* evo){
    /*for (int i =0; i< evo->essences_amount * evo->essence_len; ++i){
        if (i % evo->essence_len == 0)
            printf("\n");
        printf("%d ", evo->population[i]);
    }*/


    for (int i=0; i < evo->essences_amount; ++i) {
        if ((get_rand() / (float) RAND_MAX) < evo->mutation_factor) {
            printf("starte mutate in i = %d\n", i);

            args_mutation *args;
            args = (args_mutation*)malloc(sizeof(args_mutation));
            args->essence = &evo->population[i * evo->essence_len];
            args->essence_len = evo->essence_len;

            int err = threadpool_add(evo->threadpool, &swap_mutation, (void*) args);
            if (err!=0)
                printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA!!!!!!!!!!!!!!!!!!\n");
        }
    }

    //threadpool_barier(evo->threadpool);
    wait_all(evo->threadpool);

    /*for (int i =0; i< evo->essences_amount * evo->essence_len; ++i){
        if (i % evo->essence_len == 0)
            printf("\n");
        printf("%d ", evo->population[i]);
    }
    printf("\n");*/
}

void crossover(evolution* evo){
    printf("crossover!\n");
}


void swap_mutation(void* args){
    args_mutation* arg = (args_mutation*) args;

    int city1 = get_rand() % arg->essence_len;
    int city2 = get_rand() % arg->essence_len;

    int tmp = arg->essence[city1];
    arg->essence[city1] = arg->essence[city2];
    arg->essence[city2] = tmp;
}

void endWork(evolution* evo){
    threadpool_destroy(evo->threadpool);
}