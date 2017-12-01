//
// Created by evgeniy on 29.11.17.
//

#ifndef HOMEWORKS_5SEM_POPULATION_H
#define HOMEWORKS_5SEM_POPULATION_H

#include "graph.h"
#include "threadpool.h"
#include "myrand.h"

typedef struct evolution {
    //fields
    graph_t* rouds;              //указатель на граф дорог.
    int* population;            //популяция - маршруты
    int* children;
    int essence_len;
    int essences_amount;  //количество особей- городов
    //int best_essences;    //количество оставшихся в живых лучших особей- маршрутов после селекции// буду брать половину
    float mutation_factor;  //вероятность поменять"гены"(города) местами
    threadpool_t* threadpool;
    int minimum;
    int maximum;
    int average;
} evolution;


//methods
void evolution_init(evolution* evo, myrand_settings*, graph_t*, const int, const int, const float, int);

void selection(evolution*);
void mutation(evolution*, myrand_settings*);
void crossover(evolution*, myrand_settings*);
int minpath(evolution*);
int maxpath(evolution*);
int avgpath(evolution*);
int endWork(evolution*);

typedef struct task_mutation {
    int* essence;
    int essence_len;
    myrand_settings* rnd;
} args_mutation;

typedef struct task_caclWeight {
    int* essence;
    int essence_len;
    int ans;
    graph_t* pathes;
} args_weight;

typedef struct task_crossover {
    int* parent1;
    int* parent2;
    int parent_len;
    int* child;
    myrand_settings* rnd;
} args_crossover;

#endif //HOMEWORKS_5SEM_POPULATION_H
