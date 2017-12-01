//
// Created by evgeniy on 29.11.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "myrand.h"

//момент осознания, зачем тут статик.
//постепенное переписывание всей структуры myrand_settings сюда
static int myrand_destroyed_global;
static size_t myrand_arr_count_global;
static int** myrand_arrs_global;
static int sended_global;
//static int* arrrnd;
static int myrand_arr_size_global;
//int myrand_arr_size;
//int *arrrnd;
/*    pthread_mutex_t myrand_lock1;
    pthread_cond_t myrand_cond_full;
    pthread_cond_t myrand_cond_empty;*/

static pthread_mutex_t myrand_lock1_global = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t myrand_cond_full_global = PTHREAD_COND_INITIALIZER;
static pthread_cond_t myrand_cond_empty_global = PTHREAD_COND_INITIALIZER;

void * myrand_generator(void* void_rnd) {
    myrand_settings* rnd = (myrand_settings*) void_rnd;
    pthread_mutex_lock(&myrand_lock1_global);
    for (;;) {
        //printf("from rnd %d\n", rnd->myrand_max_arr_count);
        int *arr;
        arr = (int *) malloc(myrand_arr_size_global * sizeof(int));
        //printf("here! in generator!\n");
        while (!(myrand_destroyed_global || myrand_arr_count_global < rnd->myrand_max_arr_count)) {
            //printf("In generator. before wait\n");
            pthread_cond_wait(&myrand_cond_full_global, &myrand_lock1_global);
            //printf("here!\n");
        }
        //printf("xmmm\n");

        if (myrand_destroyed_global)
            break;

        pthread_mutex_unlock(&myrand_lock1_global);

        for (int i = 0; i < myrand_arr_size_global; ++i) {
            arr[i] = rand_r(&rnd->myrand_seed);
        }
        //printf("end write to arr\n");
        pthread_mutex_lock(&myrand_lock1_global);

        if (!myrand_arr_count_global) {
            pthread_cond_signal(&myrand_cond_empty_global);
        }
        myrand_arrs_global[myrand_arr_count_global++] = arr;
    }
    //printf("END GERERATE\n");
    pthread_mutex_unlock(&myrand_lock1_global);
}

void myrand_init(myrand_settings* rnd, int seed, int arr_size, int max_arr_count) {
    rnd = (myrand_settings*)malloc(sizeof(myrand_settings));
    myrand_destroyed_global = 0;
    myrand_arr_count_global = 0;
    myrand_arr_size_global = arr_size;
    //printf("arrsize = %d\n", arr_size);
    rnd->myrand_max_arr_count = max_arr_count;
    rnd->myrand_seed = seed;

    sended_global = myrand_arr_size_global;
    rnd->arrrnd = (int*)malloc(arr_size*sizeof(int));

    myrand_arrs_global = (int**)malloc(max_arr_count * sizeof(int*));

    pthread_create(&rnd->myrand_thread, NULL, myrand_generator, (void*)rnd);
    printf("END INIT\n");
}

void myrand_destroy(myrand_settings* rnd) {
    printf("Start destroy\n");
    pthread_mutex_lock(&myrand_lock1_global);
    myrand_destroyed_global = 1;
    //for (int i=0 ; i < 5; ++i){
    //    printf("--- %d\n", rnd->arrrnd);
    //}
    //printf("Take MUTEX\n");

//    printf("myrand_arr_count=%d\n", myrand_arr_count_global);
    while (myrand_arr_count_global != 0) {
        //printf("aa\n");
        free(myrand_arrs_global[--myrand_arr_count_global]);  //туть багь //отпути //Я сделяль
        //printf("bb\n");
    }
    free(myrand_arrs_global);
    //printf("b\n");

    free(rnd->arrrnd);
    //printf("------------c\n");

    pthread_mutex_unlock(&myrand_lock1_global);
    //printf("here3\n");
    pthread_cond_signal(&myrand_cond_full_global);
    //printf("here2\n");
    pthread_join(rnd->myrand_thread, NULL);
    //printf("section1 passed\n");
    pthread_mutex_destroy(&myrand_lock1_global);
    pthread_cond_destroy(&myrand_cond_full_global);
    pthread_cond_destroy(&myrand_cond_empty_global);
    //printf("УРА Я РАЗОБРАЛСЯ В pthreadax\n");
}

int get_myrand(myrand_settings *rnd) {
    //printf("qqq%d\n", sended_global);
    if (sended_global == myrand_arr_size_global) {
        sended_global = 0;
        //printf("qufewg\n");
        free(rnd->arrrnd);
        //printf("Get rand\n");

        pthread_mutex_lock(&myrand_lock1_global);
        while (!myrand_arr_count_global)
            pthread_cond_wait(&myrand_cond_empty_global, &myrand_lock1_global);

        rnd->arrrnd = myrand_arrs_global[--myrand_arr_count_global];

        /*printf("END RAND\n");
        int a;
        printf("arr_sz=%d", myrand_arr_size_global);
        for (int i=0; i < myrand_arr_size_global; ++i)
            printf("x =%d", rnd->arrrnd[i]);
        printf("all right\n");*/

        pthread_mutex_unlock(&myrand_lock1_global);

        pthread_cond_signal(&myrand_cond_full_global);
    }
    //if (arrrnd[sended_global++] == 0)
        //printf("~");
    //else
        //printf("+");
    return rnd->arrrnd[sended_global++];
}