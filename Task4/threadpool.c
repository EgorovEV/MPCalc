//
// Created by evgeniy on 29.11.17.
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <printf.h>

#include "threadpool.h"



static void *threadpool_thread(void *threadpool);

int threadpool_free(threadpool_t *pool);


threadpool_t* threadpool_create(int thread_count, int queue_size)
{
    threadpool_t *pool;
    pool = (threadpool_t *)malloc(sizeof(threadpool_t));    //действительно... разрушится же иначе

    // Initialize
    pool->thread_count = 0;
    pool->queue_size = queue_size;
    pool->head = pool->tail = pool->count = 0;
    pool->shutdown = 0; //==false
    pool->started = 0;

    // Allocate thread and task queue
    pool->threads = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    pool->queue = (threadpool_task_t *)malloc(queue_size * sizeof(threadpool_task_t));

    /* Initialize mutex and conditional variable first */
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->notify), NULL);


    /* Start worker threads */
    for(int i = 0; i < thread_count; i++) {
        pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool);
        pool->thread_count++;
        pool->started++;
    }

    return pool;
}

int threadpool_add(threadpool_t *pool, void (*function)(void *),void *argument) {
    //checking errors
    printf("Start adding to pool\n");
    if (pool == NULL || function == NULL) {
        return threadpool_invalid;
    }

    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    int next = (pool->tail + 1) % pool->queue_size;     //ну хоть тут будет кольцо
    //когда живешь в мск, и везде мерещаться кольца. 3+1+1+мцк+метро...

    /* Are we full ? */
    if (pool->count >= pool->queue_size)
        return threadpool_queue_full;
    /* Are we shutting down ? */
    if (pool->shutdown) {
        return threadpool_shutdown;
    }

    /* Add task to queue */
    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].argument = argument;
    pool->tail = next;
    pool->count += 1;

    /* pthread_cond_broadcast */
    pthread_cond_signal(&(pool->notify));


    if (pthread_mutex_unlock(&pool->lock) != 0) {
        return threadpool_lock_failure;
    }
    return 0;
}

int threadpool_destroy(threadpool_t* pool) {
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    /* Already shutting down */
    if (pool->shutdown)
        return threadpool_shutdown;


    pool->shutdown = 1;

    /* Wake up all worker threads */
    if (pthread_cond_broadcast(&(pool->notify)) != 0)
        return threadpool_lock_failure;


    if ((pthread_mutex_unlock(&(pool->lock)) != 0))
        return threadpool_lock_failure;

    /* Join all worker thread */
    for (int i = 0; i < pool->thread_count; i++) {
        if (pthread_join(pool->threads[i], NULL) != 0) {
            return threadpool_thread_failure;
        }
    }
    return  threadpool_free(pool);
}

int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL || pool->started > 0) {
        return -1;
    }

    /* Did we manage to allocate ? */
    if(pool->threads) {
        free(pool->threads);
        free(pool->queue);

        /* Because we allocate pool->threads after initializing the
           mutex and condition variable, we're sure they're
           initialized. Let's lock the mutex just in case. */
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}


static void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    for(;;) {
        /* Lock must be taken to wait on conditional variable */
        pthread_mutex_lock(&(pool->lock));

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        while((pool->count == 0) && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if((pool->shutdown == 1) &&
            (pool->count == 0)) {
            break;
        }

        /* Grab our task */
        task.function = pool->queue[pool->head].function;
        task.argument = pool->queue[pool->head].argument;
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count -= 1;

        /* Unlock */
        pthread_mutex_unlock(&(pool->lock));

        /* Get to work */
        printf("go to work!\n");
        (*(task.function))(task.argument);
    }

    pool->started--;

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return(NULL);
}