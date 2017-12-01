//
// Created by evgeniy on 29.11.17.
//

#ifndef HOMEWORKS_5SEM_myrand_H
#define HOMEWORKS_5SEM_myrand_H

//#include <stddef.h>
#include "pthread.h"



typedef struct myrand_settings{
/*    pthread_mutex_t myrand_lock1;
    pthread_cond_t myrand_cond_full;
    pthread_cond_t myrand_cond_empty;*/
    pthread_t myrand_thread;

    //int myrand_arr_size;
    int myrand_max_arr_count;
    //int myrand_destroyed;
    int myrand_seed;
    int myrand_arr_count;
    int *arrrnd;
    //int **myrand_arrs;

    //int sended;
}myrand_settings;




void myrand_init(myrand_settings*, int, int, int);
void myrand_destroy(myrand_settings*);

int get_myrand(myrand_settings*);

#endif //HOMEWORKS_5SEM_myrand_H
