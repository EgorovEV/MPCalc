//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include "evolution.h"
#include "myrand.h"

void swap_mutation(int*, int, int);

//граф подается на вход уже заполненным
void evolution_init(evolution* evo, graph_t* graph, const int population_size, const int old_survivals, const float mutations){
    int x = graph->n;
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
            int city1 = get_rand() % evo->essence_len;
            int city2 = get_rand() % evo->essence_len;
            printf("swap in es = %d; town1 = %d, town2 = %d\n", i, city1, city2);

            if (city1 != city2)     //если равно, то считаю, что повезло
                swap_mutation(&evo->population[i * evo->essence_len], city1, city2);

        }
    }

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


void swap_mutation(int* essence, int city1, int city2){
    int tmp = essence[city1];
    essence[city1] = essence[city2];
    essence[city2] = tmp;
}