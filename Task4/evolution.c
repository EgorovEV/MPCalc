//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include "evolution.h"
#include "myrand.h"
#include "threadpool.h"

#define NO_GENE -2
//TODO Сдалать освободение памяти!!!


void swap_mutation(void*);
void fitnes_func(void*);
void crossover_func(void*);
int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

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

    int* sorted_weight = (int*)malloc(evo->essences_amount * sizeof(int));
    for (int i = 0; i < evo->essences_amount; ++i){
        sorted_weight[i] = *weight[i];
    }

    qsort(sorted_weight, evo->essences_amount, sizeof(int), compare);

    for (int i = 0; i < evo->essences_amount; ++i){
        printf("%d ", sorted_weight[i]);
    }
    printf("\n");

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
}

void crossover(evolution* evo){
    //PMX algo
    //first step: take 2 parents; буду брать i и i+1 особь. а так же N-1 и 0-ую.
    //second: 1 parent gives half of genom to child
    //third: check func
    int* children = (int*)malloc(evo->essence_len * evo->essences_amount * sizeof(int));
    for (int i=0; i < evo->essence_len * evo->essences_amount; ++i)
        children[i] = NO_GENE;

    for (int i=0; i < evo->essences_amount; ++i){
        args_crossover *args;
        args = (args_crossover*)malloc(sizeof(args_crossover));
        args->parent1 = &evo->population[i*evo->essence_len];
        args->parent2 = &evo->population[((i+1) % (evo->essences_amount)) * evo->essence_len];
        args->parent_len = evo->essence_len;
        args->child = &children[i*evo->essence_len];
        threadpool_add(evo->threadpool, &crossover_func, (void*) args);
    }
    wait_all(evo->threadpool);
    for (int i = 0; i < evo->essence_len * evo->essences_amount; ++i){
        if (i % evo->essence_len == 0)
            printf("\n");
        printf("%d ", children[i]);
    }
    printf("\ncrossover!\n");
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

int findvalueinarray(int val, int *arr, int size){
    for (int i=0; i < size; i++) {
        if (arr[i] == val)
            return i;
    }
    return 0;
}

void crossover_func(void* args){    //todo у нас ведь всегда четная длина особи... округление вниз
    args_crossover* arg = (args_crossover*) args;
    int width = (int)(arg->parent_len*0.5);
    int start_copy = rand() % (int)(width);
    memcpy(&arg->child[start_copy], &arg->parent1[start_copy], width * sizeof(int));

    int index_in_p2;
    int gen_in_p1, ind_gen_in_p2, gen_in_p2;
    printf("in crossover\n");
    for (int i = 0; i < arg->parent_len; ++i){
        printf("%d ", arg->parent1[i]);
    }
    printf("\n");
    for (int i = 0; i < arg->parent_len; ++i){
        printf("%d ", arg->parent2[i]);
    }
    printf("\n");
    for (int i = 0; i < arg->parent_len; ++i){
        printf("%d ", arg->child[i]);
    }
    printf("\n");

    for (int i = start_copy; i < start_copy + width; ++i){      //распределяем по потомку уникальные гены род2.
        if (!findvalueinarray(arg->parent2[i], arg->child, arg->parent_len)){ //если в родителе2 нашелся новый ген(город)
            index_in_p2 = i;                                                     //берем индекс этого гена
            while (1) {                                 //пока на найдем место
                //printf("searching..\n");
                gen_in_p1 = arg->parent1[index_in_p2];                         //берем ген из род1,с индексом нового гена в род2.
                ind_gen_in_p2 = findvalueinarray(gen_in_p1, arg->parent2, arg->parent_len);   //ищем индекс этого гена в род2(во всём)
                gen_in_p2 = arg->parent2[ind_gen_in_p2];                         //берем это ген
                //printf("gen_in_p2=%d\n", gen_in_p2);
                if (!findvalueinarray(gen_in_p2, arg->child, arg->parent_len)) {   //если его ещё нет в потомке
                    arg->child[ind_gen_in_p2] = arg->parent2[i];         //то по новому индексу вставляем,
                    printf("Get it!\n");
                    for (int i = 0; i < arg->parent_len; ++i){
                        printf("%d ", arg->child[i]);
                    }
                    printf("\n");
                    break;                                                        // найденный ещё в начале, уникальный элемент
                } else {
                    //NO!!! //index_in_p2 = findvalueinarray(gen_in_p2, arg->child, arg->parent_len);
                    index_in_p2 = gen_in_p2;

                }
            }
        } else {
            printf("skip\n");
        }
    }

    for (int i=0; i < arg->parent_len; ++i){
        if(arg->child[i] == NO_GENE)
            arg->child[i] = arg->parent2[i];
    }
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
