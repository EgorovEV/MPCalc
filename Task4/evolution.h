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
    int essence_len;            //
    int essences_amount;  //количество особей- городов
    int best_essences;    //количество оставшихся в живых лучших особей- маршрутов после селекции
    float mutation_factor;  //вероятность поменять"гены"(города) местами
} evolution;

//ДА КАКОГОЖ ЧЕРТА В СИШНЫХ СТРУКТУРАХ НЕТ МЕТОДОВ?! АААААААААААА
//ГДЕ КОНСТРУКТОРЫ?! КАК СОЗДАТЬ КОНСТАНТНЫЕ ПОЛЯ? >_<

//methods
void evolution_init(evolution* evo, graph_t*, const int, const int, const float);

void selection(evolution*);
void mutation(evolution*);
void crossover(evolution*);


#endif //HOMEWORKS_5SEM_POPULATION_H
