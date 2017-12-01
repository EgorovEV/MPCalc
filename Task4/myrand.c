//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "myrand.h"

int get_rand(){
    return rand();
}


//static size_t myrand_arr_size;
//static size_t myrand_max_arr_count;

//static int **myrand_arrs;
//static int myrand_destroyed = 0;
//static unsigned int myrand_state;
//static size_t myrand_arr_count = 0;

//static pthread_t myrand_thread;
//static pthread_mutex_t myrand_lock = PTHREAD_MUTEX_INITIALIZER;
//static pthread_cond_t myrand_cond_full = PTHREAD_COND_INITIALIZER;
//static pthread_cond_t myrand_cond_empty = PTHREAD_COND_INITIALIZER;

void * myrand_generator(void* void_rnd) {
    myrand_settings* rnd = (myrand_settings*) void_rnd;

    for (;;) {
        pthread_mutex_lock(&rnd->myrand_lock);
        int *arr;
        size_t i;

        while ((rnd->myrand_arr_count < rnd->myrand_max_arr_count) || (!rnd->myrand_destroyed)) {
            pthread_cond_wait(&rnd->myrand_cond_full, &rnd->myrand_lock);
        }

        if (rnd->myrand_destroyed)
            break;

        pthread_mutex_unlock(&rnd->myrand_lock);

        for (i = 0; i < rnd->myrand_arr_size; ++i) {
            arr[i] = rand_r(&rnd->myrand_seed);
        }
        pthread_mutex_lock(&rnd->myrand_lock);

        if (!rnd->myrand_arr_count) {
            pthread_cond_signal(&rnd->myrand_cond_empty);
        }
        rnd->myrand_arrs[rnd->myrand_arr_count++] = arr;
    }
    pthread_mutex_unlock(&rnd->myrand_lock);
}
/*
void myrand_init(myrand_settings* rnd, int seed, int arr_size, int max_arr_count) {
    rnd = (myrand_settings*)malloc(sizeof(myrand_settings));
    rnd->myrand_destroyed = 0;
    rnd->myrand_arr_count = 0;
    rnd->myrand_arr_size = arr_size;
    rnd->myrand_max_arr_count = max_arr_count;
    rnd->myrand_seed = seed;

    rnd->sended = rnd->myrand_arr_size;   // = 0
    rnd->arr = (int *) malloc(rnd->myrand_arr_size * sizeof(int));;

    rnd->myrand_arrs = (int **) malloc(max_arr_count * sizeof(int *));

    pthread_create(&rnd->myrand_thread, NULL, myrand_generator, (void*)rnd);
}*/
void myrand_init() {
    printf("asd\n");
}

void myrand_destroy(myrand_settings* rnd) {
    pthread_mutex_lock(&rnd->myrand_lock);
    rnd->myrand_destroyed = 1;

    while (rnd->myrand_arr_count)
        free(rnd->myrand_arrs[--rnd->myrand_arr_count]);
    free(rnd->myrand_arrs);

    free(rnd->arr);  //?

    pthread_mutex_unlock(&rnd->myrand_lock);
    pthread_cond_signal(&rnd->myrand_cond_full);
    pthread_join(rnd->myrand_thread, NULL);

    pthread_mutex_destroy(&rnd->myrand_lock);
    pthread_cond_destroy(&rnd->myrand_cond_full);
    pthread_cond_destroy(&rnd->myrand_cond_empty);
}

int get_myrand(myrand_settings *rnd) {
    if (rnd->sended == rnd->myrand_arr_size) {
        rnd->sended = 0;
        //free(rnd->arr);       //?

        pthread_mutex_lock(&rnd->myrand_lock);
        while (!rnd->myrand_arr_count)
            pthread_cond_wait(&rnd->myrand_cond_empty, &rnd->myrand_lock);

        rnd->arr = rnd->myrand_arrs[--rnd->myrand_arr_count];
        pthread_mutex_unlock(&rnd->myrand_lock);

        pthread_cond_signal(&rnd->myrand_cond_full);
    }

    return rnd->arr[rnd->sended++];
}