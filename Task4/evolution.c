//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include "evolution.h"


//граф подается на вход уже заполненным
void evolution_init(evolution* evo, graph_t* graph, const int population_size, const int old_survivals, const int mutations){
    int x = graph->n;
    evo->essence_len = graph->n;
    evo->best_essences = old_survivals;
    evo->mutation_factor = mutations;
    evo->essences_amount = population_size;
    evo->rouds = graph;
    evo->population = (int*)malloc(population_size * graph->n * sizeof(int));
}

void selection(evolution* evo){
    printf("Selection!\n");
}

void mutation(evolution* evo){
    printf("Mutation!\n");
}

void crossover(evolution* evo){
    printf("crossover!\n");
}
