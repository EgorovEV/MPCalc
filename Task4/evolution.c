//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
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
    for (int i =0; i< evo->essences_amount * evo->essence_len; ++i){
        if (i % evo->essence_len == 0)
            printf("\n");
        printf("%d ", evo->population[i]);
    }

    for (int i=0; i < evo->essences_amount; ++i) {
        if ((get_rand() / (float) RAND_MAX) < evo->mutation_factor) {
            printf("starte mutate in i = %d\n", i);

            int mut_es = i;
            args_mutation args;
            args.mutation_essence = mut_es;     //так. почему-то не хочет создавать новый эземпляяр
            args.evo = evo;         //В итоге, во всех потоках лежит одинаковое значение особи

            //добавить чекер на ошибки
            threadpool_add(evo->threadpool, &swap_mutation, (void*) &args);
        }
    }

    threadpool_destroy(evo->threadpool);  //TODO убрать отсюда в конец работы

    for (int i =0; i< evo->essences_amount * evo->essence_len; ++i){
        if (i % evo->essence_len == 0)
            printf("\n");
        printf("%d ", evo->population[i]);
    }
    printf("\n");
}

void crossover(evolution* evo){
    printf("crossover!\n");
}


void swap_mutation(void* args){
    args_mutation* arg = (args_mutation*) args;

    int city1 = get_rand() % arg->evo->essence_len;
    int city2 = get_rand() % arg->evo->essence_len;

    printf("swap in es = %d; town1 = %d, town2 = %d\n", arg->mutation_essence, city1, city2);
    int tmp = arg->evo->population[arg->mutation_essence * arg->evo->essence_len + city1];
    arg->evo->population[arg->mutation_essence * arg->evo->essence_len + city1]
            = arg->evo->population[arg->mutation_essence * arg->evo->essence_len + city2];
    arg->evo->population[arg->mutation_essence * arg->evo->essence_len + city2] = tmp;
}