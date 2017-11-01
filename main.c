/* C program for parallel Sort */
/* using pthread*/
/* I take an idea of algorithm from this article, "section IV":
 * http://ieeexplore.ieee.org/document/7036012/?part=1
 */

#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void printArray(int A[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

typedef struct args{
    int* arr_begin;
    int l;
    int r;
    int* L;
    int* R;
    int n1;
    int n2;
} args_for_merge;

typedef struct paralSortArgs{
    int* arr;
    int chank_sz, l, r, depth;
    int threadsExist, max_threads;
} paralSortArgs;

void mysort(int *arr, int l, int r){
    qsort(&arr[l], r - l + 1, sizeof(int), compare);
}


void* merge_from_left(void *args){

    args_for_merge *chank = (args_for_merge*) args;

    int i_l, j_l, k_l;
    i_l = 0; // Initial index of first subarray goes from left
    j_l = 0; // Initial index of second subarray goes from left
    k_l = chank->l; // Initial index of merged subarray goes from left

    while (42 == 42) {
        if (chank->L[i_l] < chank->R[j_l]) {
            *(chank->arr_begin + k_l) = chank->L[i_l];
            ++i_l;
            ++k_l;
        } else {
            *(chank->arr_begin + k_l) = chank->R[j_l];
            ++j_l;
            ++k_l;
        }
        if (j_l >= chank->n2 || i_l >= chank->n1) {
            break;
        }
    }
}


void* merge_from_right(void *args) {
    args_for_merge *chank = (args_for_merge *) args;

    int i_r, j_r, k_r;
    i_r = chank->n1 - 1; // Initial index of first subarray goes from right
    j_r = chank->n2 - 1; // Initial index of second subarray goes from right
    k_r = chank->r; // Initial index of merged subarray goes from right

    while (42 == 42) {
        if (chank->L[i_r] >= chank->R[j_r]) {
            *(chank->arr_begin + k_r) = chank->L[i_r];
            --i_r;
            --k_r;
        } else {
            *(chank->arr_begin + k_r) = chank->R[j_r];
            --j_r;
            --k_r;
        }
        if (j_r < 0 || i_r < 0) {
            break;
        }
    }
}

void merge(int *arr, int left_bounder, int middle, int right_bounder, int max_thread)
{
    args_for_merge current_chank;

    current_chank.l = left_bounder;
    current_chank.r = right_bounder;
    current_chank.arr_begin = &arr[0];
    current_chank.n1 = middle - left_bounder + 1;
    current_chank.n2 = right_bounder - middle;

    current_chank.L = (int*)malloc(current_chank.n1 * sizeof(int));
    current_chank.R = (int*)malloc(current_chank.n2 * sizeof(int));

    memcpy(current_chank.L, &arr[left_bounder], current_chank.n1 * sizeof(int));
    memcpy(current_chank.R, &arr[middle +1], current_chank.n2 * sizeof(int));

    if (max_thread > 1) {

        pthread_t threads[2];
        pthread_create(&threads[0], NULL, merge_from_left, (void *) &current_chank);
        pthread_create(&threads[1], NULL, merge_from_right, (void *) &current_chank);

        for (int i = 0; i < 2; ++i) {
            pthread_join(threads[i], NULL);
        }
    }
    else{
        merge_from_left((void*) &current_chank);
        merge_from_right((void*) &current_chank);
    }
    free(current_chank.L);
    free(current_chank.R);
}



void* initParallelSort(void* args){
    paralSortArgs *arrArgs = (paralSortArgs *) args;

    if (arrArgs->r - arrArgs->l + 1> arrArgs->chank_sz) {
        int m = arrArgs->l + (arrArgs->r - arrArgs->l) / 2;

        paralSortArgs args[2];
        args[0].arr = arrArgs->arr;
        args[0].chank_sz = arrArgs->chank_sz;
        args[0].l = arrArgs->l;
        args[0].r = m;
        args[0].depth = arrArgs->depth + 1;
        args[0].threadsExist = arrArgs->threadsExist * 2;
        args[0].max_threads = arrArgs->max_threads;
        args[1].arr = arrArgs->arr;
        args[1].chank_sz = arrArgs->chank_sz;
        args[1].l = m + 1;
        args[1].r = arrArgs->r;
        args[1].depth = arrArgs->depth + 1;
        args[1].threadsExist = arrArgs->threadsExist * 2;
        args[1].max_threads = arrArgs->max_threads;

        if (arrArgs->threadsExist < arrArgs->max_threads) {
            pthread_t threads[2];
            for (int i = 0; i < 2; ++i) {
                pthread_create(&threads[i], NULL, initParallelSort, (void*) &args[i]);
            }
            pthread_join(threads[0], NULL);
            pthread_join(threads[1], NULL);
        }
        else {
            initParallelSort((void*) &args[0]);
            initParallelSort((void*) &args[1]);
        }
        merge(arrArgs->arr, arrArgs->l, m, arrArgs->r, arrArgs->max_threads);
    }else{
        mysort(arrArgs->arr, arrArgs->l, arrArgs->r);
    }
}

int main(int argc, char* argv[]) {
    int arr_size, chank_size, P;
    arr_size = atoi(argv[1]);
    chank_size = atoi(argv[2]);
    P = atoi(argv[3]);

    int *arr = (int *) malloc(sizeof(int) * arr_size);

    int *arr2 = (int *) malloc(sizeof(int) * arr_size);
    srand(time(NULL));

//create array
    for (int i = 0; i < arr_size; ++i) {
        arr[i] = rand();
        //arr[i] = rand() % 10000;  //"%10000" is special for data.txt, for good view;
        arr2[i] = arr[i];
    }
//write array to data.txt
    FILE *file;
    file = fopen("data.txt", "a");
    if (file == NULL) {
        printf("Can't open data.txt!\n");
        return 8;
    }

    for (int i = 0; i < arr_size; ++i)
        fprintf(file, "%d ", arr[i]);
    fprintf(file, "\n");

//sorting, using OMP
    double whole_time = omp_get_wtime();
    paralSortArgs args;
    args.arr = arr;
    args.chank_sz = chank_size;
    args.l = 0;
    args.r = arr_size - 1;
    args.depth = 0;
    args.threadsExist = 1;
    args.max_threads = P;

    initParallelSort((void *) &args);

    double whole_time_of_parallelSort = omp_get_wtime() - whole_time;
    printf("parallel sort work: %f\n", whole_time_of_parallelSort);

//sorting, using library qsort
    whole_time = omp_get_wtime();
    qsort(arr2, arr_size, sizeof(int), compare);
    whole_time = omp_get_wtime() - whole_time;
    printf("library quicksort work: %f\n", whole_time);

//checking errors
    for (int i = 0; i < arr_size; ++i)
        if (arr[i] != arr2[i])
            printf("Всё пропало!!!\n");

//writing sorted array to data.txt
    for (int i = 0; i < arr_size; ++i)
        fprintf(file, "%d ", arr[i]);
    fprintf(file, "\n\n");
    fclose(file);

//writing statistic
    FILE *file2;
    file2 = fopen("stats.txt", "a");
    if (file2 == NULL) {
        printf("Can't open file!\n");
        return 8;
    }
    fprintf(file2, "%f %f %d %d %d \n", whole_time_of_parallelSort, whole_time, \
        arr_size, chank_size, P);
    fclose(file2);

    //   }
    //}
    free(arr);
    free(arr2);
    return 0;
}
