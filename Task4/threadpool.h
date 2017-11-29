//
// Created by evgeniy on 29.11.17.
//
//хватит изобретать велосипеды. Этим занимался в предыдущем семестре.

#ifndef HOMEWORKS_5SEM_THREADPOOL_H
#define HOMEWORKS_5SEM_THREADPOOL_H

#include <pthread.h>

typedef enum {
    threadpool_invalid        = -1,
    threadpool_lock_failure   = -2,
    threadpool_queue_full     = -3,
    threadpool_shutdown       = -4,
    threadpool_thread_failure = -5
} threadpool_error_t;


typedef struct queue_implementation{
    void (*function)(void *);
    void *argument;
} threadpool_task_t;


typedef struct threadpool_t {
    pthread_mutex_t lock;
    pthread_cond_t notify;      //местный conditional veriable
    pthread_t *threads;
    threadpool_task_t *queue;
    int thread_count;           //количество потоков
    int queue_size;             //Размер очереди
    int head;                   //индекс 1-го эл-та
    int tail;                   //индекс след. л-та
    int count;                  //количество ожидающих потоков
    int shutdown;               //Индикатор завершения работы-> "удаляем очередь"
    int started;                //количество стартовых потоков
} threadpool_t;

threadpool_t* threadpool_create(int, int);
int threadpool_add(threadpool_t*, void (*function)(void *),void*);
int threadpool_destroy(threadpool_t*);





#endif //HOMEWORKS_5SEM_THREADPOOL_H
