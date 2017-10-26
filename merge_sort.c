/* C program for parallel Sort */
//why concequence of headers is important
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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

void mysort(int *arr, int l, int r){
    qsort(&arr[l], r - l, sizeof(int), compare);
}

void merge(int *arr, int arr_size, int chank_size, int l, int m, int r)
{
	int i_l, j_l, k_l, i_r, j_r, k_r;

	int n1 = m - l + 1;
	int n2 = r - m;
    int amount_of_responsible_elements = chank_size / 2;
    // create temp arrays
	//int L[n1], R[n2];
	int *L = NULL;
	int *R = NULL;
	L = (int*)malloc(n1 * sizeof(int));
	R = (int*)malloc(n2 * sizeof(int));


	
	for (int i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (int j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	i_l = 0; // Initial index of first subarray go from left
	j_l = 0; // Initial index of second subarray go from left
	k_l = l; // Initial index of merged subarray go from left
    i_r = n1; // Initial index of first subarray go from right
    j_r = n2; // Initial index of second subarray go from right
    k_r = r; // Initial index of merged subarray go from right


#pragma omp task shared(i_l, j_l) firstprivate(amount_of_responsible_elements)
    {
        while (amount_of_responsible_elements > 0) {
            --amount_of_responsible_elements;
            if (L[i_l] < R[j_l]) {
                arr[k_l] = L[i_l];
                ++i_l;
                ++k_l;
            } else {
                arr[k_l] = R[j_l];
                ++j_l;
                ++k_l;
            }
        }
    }

#pragma omp task shared(i_r, j_r) firstprivate(amount_of_responsible_elements)
    {
        while (amount_of_responsible_elements > 0) {
            --amount_of_responsible_elements;
            if (L[i_r] > R[j_r]) {
                arr[k_r] = L[i_r];
                --i_r;
                --k_r;
            } else {
                arr[k_r] = R[j_r];
                --j_r;
                --k_r;
            }
            if (j_r == -1 || i_r == n2)
                break;
        }
    }

	while (i_l < i_r) {
        arr[k_l] = L[i_l];
        ++k_l;
        ++i_l;
    }
    while (j_l < j_r) {
        arr[k_r] = R[j_l];
        --k_r;
        ++i_l;
    }

	free(L);
	free(R);
}


void parallelSort(int *arr, int arr_size, int chank_size, int l, int r) {
    if (r - l > chank_size) {
        int m = l + (r - l) / 2;
#pragma omp task
        parallelSort(arr, arr_size, chank_size, l, m);
#pragma omp task
        parallelSort(arr, arr_size, chank_size, m + 1, r);
#pragma omp taskwait
        merge(arr, arr_size, chank_size, l, m, r);
    }else{
        mysort(arr, l, r);
    }
}


int main(int argc, char* argv[])
{
    int arr_size, chank_size, P;
    arr_size = atoi(argv[1]);
    chank_size = atoi(argv[2]);
    P = atoi(argv[3]);

	int *arr = (int*)malloc(sizeof(int) * arr_size);
    srand(time(NULL));

    for (int i = 0; i < arr_size; ++i){
        arr[i] = rand() % 100;
    }

    double whole_time = omp_get_wtime( );
#pragma omp parallel
    {
#pragma omp single
        parallelSort(arr, arr_size, chank_size, 0, arr_size - 1);
    }
    whole_time = omp_get_wtime() - whole_time;
    printf("parallel sort work: %f\n", whole_time);


    for (int i = 0; i < arr_size; ++i){
        arr[i] = rand() % 100;
    }
    whole_time = omp_get_wtime();
    qsort(arr, arr_size, sizeof(int), compare);
    whole_time = omp_get_wtime() - whole_time;
    printf("library quicksort work: %f\n", whole_time);
	return 0;
}