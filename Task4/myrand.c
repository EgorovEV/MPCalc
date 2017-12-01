//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "myrand.h"


static size_t myrand_arr_count_global;

//static size_t myrand_arr_size;
//static size_t myrand_max_arr_count;


void * myrand_generator(void* void_rnd) {
    myrand_settings* rnd = (myrand_settings*) void_rnd;
    pthread_mutex_lock(&rnd->myrand_lock1);
    printf("from rnd %d\n", rnd->myrand_max_arr_count);
    for (;;) {
        int *arr;
        arr = (int *) malloc(rnd->myrand_arr_size * sizeof(int));
        printf("here! in generator!\n");
        while (!(rnd->myrand_destroyed || myrand_arr_count_global < rnd->myrand_max_arr_count)) {
            printf("In generator. before wait\n");
            pthread_cond_wait(&rnd->myrand_cond_full, &rnd->myrand_lock1);
            printf("here!\n");
        }
        printf("xmmm\n");

        if (rnd->myrand_destroyed)
            break;

        pthread_mutex_unlock(&rnd->myrand_lock1);

        for (int i = 0; i < rnd->myrand_arr_size; ++i) {
            arr[i] = rand_r(&rnd->myrand_seed);
            //printf("o here\n");
        }
        printf("end write to arr\n");
        pthread_mutex_lock(&rnd->myrand_lock1);

        if (!myrand_arr_count_global) {
            pthread_cond_signal(&rnd->myrand_cond_empty);
        }
        rnd->myrand_arrs[myrand_arr_count_global] = arr;
        myrand_arr_count_global+=1;
//        printf("end? starts new step myrand_arr_count = %d!\n", myrand_arr_count_global);
    }
    printf("END GERERATE\n");
    pthread_mutex_unlock(&rnd->myrand_lock1);
}

void myrand_init(myrand_settings* rnd, int seed, int arr_size, int max_arr_count) {
    rnd = (myrand_settings*)malloc(sizeof(myrand_settings));
    rnd->myrand_destroyed = 0;
    myrand_arr_count_global = 0;
    rnd->myrand_arr_size = arr_size;
    printf("arrsize = %d\n", arr_size);
    rnd->myrand_max_arr_count = max_arr_count;
    rnd->myrand_seed = seed;

    rnd->sended = rnd->myrand_arr_size;
    rnd->arrrnd = (int*)malloc(arr_size*sizeof(int));

    rnd->myrand_arrs = (int**)malloc(max_arr_count * sizeof(int*));

    pthread_create(&rnd->myrand_thread, NULL, myrand_generator, (void*)rnd);
    printf("END INIT\n");
}

void myrand_destroy(myrand_settings* rnd) {
    printf("Start destroy\n");
    pthread_mutex_lock(&rnd->myrand_lock1);
    rnd->myrand_destroyed = 1;

    printf("Take MUTEX\n");

//    printf("myrand_arr_count=%d\n", myrand_arr_count_global);
    while (myrand_arr_count_global != 0) {
        printf("aa\n");
        free(rnd->myrand_arrs[--myrand_arr_count_global]);  //туть багь
        printf("bb\n");
    }
    //free(rnd->myrand_arrs);
    printf("b\n");

    free(rnd->arrrnd);
    printf("------------c\n");

    pthread_mutex_unlock(&rnd->myrand_lock1);
    //pthread_cond_signal(&rnd->myrand_cond_full);
    pthread_join(rnd->myrand_thread, NULL);
    printf("section1 passed\n");
    pthread_mutex_destroy(&rnd->myrand_lock1);
    //pthread_cond_destroy(&rnd->myrand_cond_full);
    pthread_cond_destroy(&rnd->myrand_cond_empty);
}

int get_myrand(myrand_settings *rnd) {
    if (rnd->sended == rnd->myrand_arr_size) {
        rnd->sended = 0;
        free(rnd->arrrnd);
        printf("Get rand\n");

        pthread_mutex_lock(&rnd->myrand_lock1);
        while (!myrand_arr_count_global)
            pthread_cond_wait(&rnd->myrand_cond_empty, &rnd->myrand_lock1);

        rnd->arrrnd = rnd->myrand_arrs[--myrand_arr_count_global];
        pthread_mutex_unlock(&rnd->myrand_lock1);

        pthread_cond_signal(&rnd->myrand_cond_full);
    }

    return rnd->arrrnd[rnd->sended++];
}