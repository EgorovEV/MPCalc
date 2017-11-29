//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include "evolution.h"
#include "myrand.h"

//граф подается на вход уже заполненным
void evolution_init(evolution* evo, graph_t* graph, const int population_size, const int old_survivals, const float mutations){
    int x = graph->n;
    evo->essence_len = graph->n;
    evo->best_essences = old_survivals;
    evo->mutation_factor = mutations;
    evo->essences_amount = population_size;
    evo->rouds = graph;
    evo->population = (int*)malloc(population_size * graph->n * sizeof(int));
    //TODO ТЕСТОВАЯ ВЫЮОРКА!!!
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
            //зафигачить в тредпул!
            //а пока что, втупую, ибо 4 час ночи
            int tmp = evo->population[i * evo->essence_len + city1];
            evo->population[i * evo->essence_len + city1] = evo->population[i * evo->essence_len + city2];
            evo->population[i * evo->essence_len + city2] = tmp;
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
