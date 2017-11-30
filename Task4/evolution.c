//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <zconf.h>
#include <string.h>
#include <math.h>
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
int* get_best(int*, int);

void fulfillPopulation(evolution*);
//граф подается на вход уже заполненным
void evolution_init(evolution* evo, graph_t* graph, const int population_size, const int old_survivals, const float mutations, int threads){
    evo->essence_len = graph->n;
    //evo->best_essences = old_survivals;
    evo->mutation_factor = mutations;
    evo->essences_amount = population_size;
    evo->rouds = graph;
    evo->population = (int*)malloc(population_size * graph->n * sizeof(int));
    evo->children = (int*)malloc(population_size * graph->n * sizeof(int));

    fulfillPopulation(evo);

    evo->threadpool = threadpool_create(threads, 10000);
    evo->minimum = 2100000000;
}

void selection(evolution* evo) {
    //printf("nachali!\n");
    int **weight = (int **) malloc(evo->essences_amount * sizeof(int *));
    int *type_essence[2];
    type_essence[0] = evo->population;
    type_essence[1] = evo->children;

    /*printf("\n\n______________POPULATION_____________\n");
    for (int i = 0; i < evo->essence_len * evo->essences_amount; ++i) {
        if (i % evo->essence_len == 0) {
            printf("\n");
            //printf("  -> w = %d\n", *weight[i / evo->essence_len]);
        }
        printf("%d ", evo->population[i]);
    }
    printf("\n\n_______________CHILDREN______________\n");
    for (int i = 0; i < evo->essence_len * evo->essences_amount; ++i) {
        if (i % evo->essence_len == 0) {
            printf("\n");
            //printf("  -> w = %d\n", *weight[i / evo->essence_len]);
        }
        printf("%d ", evo->children[i]);
    }
    printf("\n\n");*/

    int tmp_for_end_print = -1;
    int minim = 1000000;
    int max = 0;
    int avg = 0;
    //шаг 1.
    int counter_new_essence = 0;
    int *new_generation = (int *) malloc(evo->essence_len * evo->essences_amount * sizeof(int));
    for (int i =0; i < evo->essence_len*evo->essences_amount; ++i)
        new_generation[i] = -3;

    for (int type_es = 0; type_es < 2; ++type_es) {
        for (int i = 0; i < evo->essences_amount; ++i) {
            args_weight *args;
            args = (args_weight *) malloc(sizeof(args_weight));
            args->essence_len = evo->essence_len;
            //args->essence = &evo->population[i * evo->essence_len];
            args->essence = &((type_essence[type_es])[i * evo->essence_len]);
            args->ans = 0;
            args->pathes = evo->rouds;

            threadpool_add(evo->threadpool, &fitnes_func, (void *) args);
            weight[i] = &args->ans;
        }
        wait_all(evo->threadpool);



        //if (type_es == 1){

        //}


        //printf("end with threadpool in type %d\n", type_es);

        int *sorted_weight = (int *) malloc(evo->essences_amount * sizeof(int));

        for (int i = 0; i < evo->essences_amount; ++i) {
            sorted_weight[i] = *weight[i];
            avg += sorted_weight[i];
            if (sorted_weight[i] < minim)
                minim = sorted_weight[i];
            if (sorted_weight[i] > max)
                max = sorted_weight[i];
        }
        avg = avg/(evo->essences_amount);
        //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MINIM = %d", minim);

        qsort(sorted_weight, evo->essences_amount, sizeof(int), compare);

        //printf("~~~~~~w:  ");
        //for (int k = 0; k < evo->essences_amount; ++k)
        //    printf("%d ", *weight[k]);
        //printf("\n");
        int best_essences = evo->essences_amount / 2;
        if (type_es == 1 && (evo->essences_amount) % 2) {
            ++best_essences;
        }
        int suitable_max = sorted_weight[best_essences];

        //printf("-----sutable_max = %d\n", suitable_max);

        tmp_for_end_print = sorted_weight[0];
        /*for (int i = 0; i < evo->essences_amount; ++i) {
            printf("%d ", sorted_weight[i]);
        }
        printf("\n");*/
        //printf("prepare for copy!\n");

        int *edge = (int*)malloc(evo->essences_amount * sizeof(int));
        int counter_edge = 0;


        for (int i = 0; i < evo->essences_amount; ++i) {
            if ((*weight[i] < suitable_max) && (best_essences)) {
                //printf("hello!\n");
                memcpy(&new_generation[counter_new_essence * evo->essence_len],
                       &((type_essence[type_es])[i * evo->essence_len]), (size_t) evo->essence_len * sizeof(int));
                //printf("~~~~~~~~~~~~I COPY:~~~~~~~~~~~~~~~~~~~\n");
                //for(int j = 0; j < evo->essence_len; ++j){
                //    printf("%d ", (type_essence[type_es])[i * evo->essence_len + j]);
                //}
                //printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                //printf("hi\n");
                ++counter_new_essence;
                --best_essences;
            }
            if ((*weight[i] == suitable_max) && (best_essences))
                edge[counter_edge++] = i;                           //++
        }




        if (best_essences != 0) {   //т.к. если сразу копировать пути с граничными весами, они могут заменить некоторые
                //маленькие веса, т.к. копируем по порядку в неотсортированном массиве.
            int inserted = 0;
            while (best_essences > 0) {
                memcpy(&new_generation[counter_new_essence * evo->essence_len],
                       &(type_essence[type_es])[edge[inserted] * evo->essence_len], (size_t) evo->essence_len * sizeof(int));    //segf?


                //printf("~~~~~~~~~~~~I ADDITONAL COPY:~~~~~~~~~~~~~~~~~~~\n");
                /*for(int j = 0; j < evo->essence_len; ++j){
                    printf("%d ", (type_essence[type_es])[edge[inserted] * evo->essence_len + j]);
                }*/
                //printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

                ++inserted;
                --best_essences;
                ++counter_new_essence;
            }
        }
        free(edge);
        free(sorted_weight);
        //printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    }


    memcpy(&evo->population[0], &new_generation[0], evo->essence_len * evo->essences_amount * sizeof(int));

    if (minim <= evo->minimum) {
        evo->minimum = minim;
    }
    evo->maximum = max;
    evo->average = avg;

    free(weight);
    free(new_generation);

}

void crossover(evolution* evo){
    //PMX algo
    //first step: take 2 parents; буду брать i и i+1 особь. а так же N-1 и 0-ую.
    //second: 1 parent gives half of genom to child
    //third: check func
    for (int i=0; i < evo->essence_len * evo->essences_amount; ++i) //new generation- new genes
        evo->children[i] = NO_GENE;

    for (int i=0; i < evo->essences_amount; ++i){
        args_crossover *args;
        args = (args_crossover*)malloc(sizeof(args_crossover));
        args->parent1 = &evo->population[i*evo->essence_len];
        args->parent2 = &evo->population[((i+1) % (evo->essences_amount)) * evo->essence_len];
        args->parent_len = evo->essence_len;
        args->child = &evo->children[i*evo->essence_len];
        threadpool_add(evo->threadpool, &crossover_func, (void*) args);
    }
    wait_all(evo->threadpool);
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

    //printf("\n\n");
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
    answer+= graph_weight(arg->pathes, arg->essence[arg->essence_len-1], arg->essence[0]);
    arg->ans = answer;
}

int findvalueinarray(int val, int *arr, int size){
    for (int i=0; i < size; i++) {
        if (arr[i] == val)
            return i;
    }
    return -4;
}

void crossover_func(void* args){    //todo у нас ведь всегда четная длина особи.. nope. округление вниз
    args_crossover* arg = (args_crossover*) args;
    int width = (int)ceil(arg->parent_len*0.5);
    int start_copy = rand() % (int)(width);
    memcpy(&arg->child[start_copy], &arg->parent1[start_copy], width * sizeof(int));

    int index_in_p2;
    int gen_in_p1, ind_gen_in_p2;

    for (int i = start_copy; i < start_copy + width; ++i){      //распределяем по потомку уникальные гены род2.
        if (findvalueinarray(arg->parent2[i], arg->child, arg->parent_len) == -4){ //если в родителе2 нашелся новый ген(город)
            index_in_p2 = i;                                                     //берем индекс этого гена
            int max_steps = width; //от зацикливания, в случае полного совпадения "генов"
            while (max_steps-- > 0) {                                 //пока на найдем место
                gen_in_p1 = arg->parent1[index_in_p2];                         //берем ген из род1,с индексом нового гена в род2.
                ind_gen_in_p2 = findvalueinarray(gen_in_p1, arg->parent2, arg->parent_len);   //ищем индекс этого гена в род2(во всём)
                if (arg->child[ind_gen_in_p2] == NO_GENE){               //если тут ген потомка ещё не определен
                    arg->child[ind_gen_in_p2] = arg->parent2[i];         //то по новому индексу вставляем,
                    break;                                                     // найденный ещё в начале, уникальный элемент
                } else {
                    index_in_p2 = ind_gen_in_p2;
                }
            }
        }
    }
    for (int i=0; i < arg->parent_len; ++i){
        if(arg->child[i] == NO_GENE)
            arg->child[i] = arg->parent2[i];
    }
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

int minpath(evolution* evo){
    return evo->minimum;
}
int maxpath(evolution* evo){
    return evo->maximum;
}
int avgpath(evolution* evo){
    return evo->average;
}

int endWork(evolution* evo){
    free(evo->children);
    free(evo->population);

    threadpool_destroy(evo->threadpool);
    return evo->minimum;
}
