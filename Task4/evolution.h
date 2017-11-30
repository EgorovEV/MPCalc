//
// Created by evgeniy on 29.11.17.
//

#ifndef HOMEWORKS_5SEM_POPULATION_H
#define HOMEWORKS_5SEM_POPULATION_H

#include "graph.h"
#include "threadpool.h"

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
} evolution;

//ДА КАКОГОЖ ЧЕРТА В СИШНЫХ СТРУКТУРАХ НЕТ МЕТОДОВ?! АААААААААААА

//methods
void evolution_init(evolution* evo, graph_t*, const int, const int, const float, int);

void selection(evolution*);
void mutation(evolution*);
void crossover(evolution*);
int minpath(evolution*);
int endWork(evolution*);

typedef struct task_mutation {
    int* essence;
    int essence_len;
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
} args_crossover;

#endif //HOMEWORKS_5SEM_POPULATION_H
