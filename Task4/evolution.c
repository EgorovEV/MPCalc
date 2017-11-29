//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <zconf.h>
#include "evolution.h"
#include "myrand.h"
#include "threadpool.h"
//TODO Сдалать освободение памяти!!!

void swap_mutation(void*);
void fitnes_func(void*);

void fulfillPopulation(evolution*);
//граф подается на вход уже заполненным
void evolution_init(evolution* evo, graph_t* graph, const int population_size, const int old_survivals, const float mutations, int threads){
    evo->essence_len = graph->n;
    evo->best_essences = old_survivals;
    evo->mutation_factor = mutations;
    evo->essences_amount = population_size;
    evo->rouds = graph;
    evo->population = (int*)malloc(population_size * graph->n * sizeof(int));

    fulfillPopulation(evo);

    evo->threadpool = threadpool_create(threads, 100);
}

void selection(evolution* evo){
    int* best_eseences_ind;
    best_eseences_ind = (int*)malloc(evo->best_essences * sizeof(int));
    for (int i =0; i < evo->best_essences; ++i)
        best_eseences_ind[i] = 0; // back to school
    //вычисляю фитнес функцию записываю по индесу (под номерм i вес i-ой особи)
    //вычисляю x: #весов < x есть best_essences;
    //забираю лучших

    int** weight = (int**)malloc(evo->essences_amount * sizeof(int*));

    for (int i = 0; i < evo->essences_amount; ++i){
        args_weight *args;
        args = (args_weight*)malloc(sizeof(args_weight));
        args->essence_len = evo->essence_len;
        args->essence = &evo->population[i * evo->essence_len];
        args->ans = 0;
        args->pathes = evo->rouds;

        threadpool_add(evo->threadpool, &fitnes_func, (void*) args);
        weight[i] = &args->ans;
    }
    wait_all(evo->threadpool);
    for (int i = 0; i < evo->essences_amount; ++i){
        printf("ans = %d\n", *weight[i]);
    }
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
            //printf("starte mutate in i = %d\n", i);

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

void fitnes_func(void* args){
    args_weight* arg = (args_weight*) args;
    int answer = 0;
    for (int i = 0; i < arg->essence_len - 1; ++i){
        int w = graph_weight(arg->pathes, arg->essence[i], arg->essence[i+1]);
        if (w!=-1)
            answer += w;
    }
    arg->ans = answer;
    printf("In fitn, and = %d\n", arg->ans);
}


void endWork(evolution* evo){
    threadpool_destroy(evo->threadpool);
}

void fulfillPopulation(evolution* evo){
    const int array_size = evo->essence_len;
    int t = 0;
    int k = 0;
    for (int j = 0; j < evo->essences_amount; ++j) {
        for (int i = 0; i < array_size; ++i) //заполняем масив последовательными числами от 0 до 14 и выводим их
            evo->population[j * array_size + i] = i;

        for (int i = 0; i < array_size; ++i) {
            k = rand() % (array_size - 1);
//меняем местами два элемента: текущий и елемент со случайним номером
            t = evo->population[j * array_size + i];
            evo->population[j * array_size + i] = evo->population[j * array_size + k];
            evo->population[j * array_size + k] = t;
        }
    }
 }
