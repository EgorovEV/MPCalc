/* C program for parallel Sort */
/* using pthread*/
/* I take an idea of algorithm from this article, "section IV":
 * http://ieeexplore.ieee.org/document/7036012/?part=1
 */

#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#define NUM_THREADS 2


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

void mysort(int *arr, int l, int r){
    qsort(&arr[l], r - l + 1, sizeof(int), compare);
}


void* merge_from_left(void *args){

    args_for_merge *chank = (args_for_merge*) args;
    int amount_of_responsible_elements = (chank->r - chank->l) / 2 + 1;

    int i_l, j_l, k_l;
    i_l = 0; // Initial index of first subarray goes from left
    j_l = 0; // Initial index of second subarray goes from left
    k_l = chank->l; // Initial index of merged subarray goes from left

    while (amount_of_responsible_elements > 0) {
        --amount_of_responsible_elements;
        if (chank->L[i_l] < chank->R[j_l]) {
            *(chank->arr_begin + k_l) = chank->L[i_l];
            ++i_l;
            ++k_l;
        } else {
            *(chank->arr_begin + k_l) = chank->R[j_l];
            ++j_l;
            ++k_l;
        }
    }
}


void* merge_from_right(void *args) {
    args_for_merge *chank = (args_for_merge *) args;

    int amount_of_responsible_elements = (chank->r - chank->l) / 2 + 1;

    int i_r, j_r, k_r;
    i_r = chank->n1 - 1; // Initial index of first subarray goes from right
    j_r = chank->n2 - 1; // Initial index of second subarray goes from right
    k_r = chank->r; // Initial index of merged subarray goes from right

    while (amount_of_responsible_elements > 0) {
        --amount_of_responsible_elements;
        if (chank->L[i_r] > chank->R[j_r]) {
            *(chank->arr_begin + k_r) = chank->L[i_r];
            --i_r;
            --k_r;
        } else {
            *(chank->arr_begin + k_r) = chank->R[j_r];
            --j_r;
            --k_r;
        }
    }
}

void merge(int *arr, int left_bounder, int middle, int right_bounder)
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

    pthread_t for_left;
    pthread_t for_right;

    pthread_create(&for_left, NULL, merge_from_left, (void*) &current_chank);
    pthread_create(&for_right, NULL, merge_from_right, (void*) &current_chank);
    int status_addr;
    pthread_join(for_left, (void**)&status_addr);
    pthread_join(for_right, (void**)&status_addr);

    free(current_chank.L);
    free(current_chank.R);
}

struct paralSortArgs{
    int* arr;
    int chank_sz, l, r, depth;
};

void parallelSort(int *arr, int chank_size, int l, int r, int depth) { //разбить на 1 шаг, и пооследующие
    if (r - l > chank_size) {
        int m = l + (r - l) / 2;

        /*if (depth == 0) {
            pthread_t threads[NUM_THREADS];     //I'm tired
            paralSortArgs args[2];
            args[0].arr = arr;
            args[0].chank_sz = chank_size;
            args[0].l = l;
            args[0].r = m;
            args[0].depth = depth + 1;
            args[1].arr = arr;
            args[1].chank_sz = chank_size;
            args[1].l = m + 1;
            args[1].r = r;
            args[1].depth = depth + 1;

            for (int i = 0; i < NUM_THREADS; ++i) {
                pthread_create(&threads[i], NULL, parallelSort, (void *) &args[i]);
            }
        }*/
        parallelSort(arr, chank_size, l, m, depth+1);
        parallelSort(arr, chank_size, m + 1, r, depth+1);
        merge(arr, l, m, r);
    }else{
        mysort(arr, l, r);
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
        //arr[i] = rand() % 100;  //"%10000" is special for data.txt, for good view;
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
    omp_set_num_threads(P);
    double whole_time = omp_get_wtime();

    //printArray(arr, arr_size);
    //printf("ready\n");
    parallelSort(arr, chank_size, 0, arr_size - 1, 0);
    //printf("\n\ndone\n");

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
    if (file == NULL) {
        printf("Can't open file!\n");
        return 8;
    }
    fprintf(file2, "%f %f %d %d %d \n", whole_time_of_parallelSort, whole_time, \
        arr_size, chank_size, P);
    fclose(file2);

    return 0;
}
