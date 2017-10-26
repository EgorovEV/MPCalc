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

void merge(int arr[], int l, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
    qsort(arr, r - l + 1 , sizeof(int), compare);
    // create temp arrays
	//int L[n1], R[n2];
	int *L = NULL;
	int *R = NULL;
	L = (int*)malloc(n1 * sizeof(int));
	R = (int*)malloc(n2 * sizeof(int));


	
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
	free(L);
	free(R);
}

void merge(int *arr, int arr_size, int chank_size, int threads_amount, int chank_amount){
    int l = 0;
    int r = chank_size;
    int m = r - (r - l) / 2;
    for (i = 0; i < chank_amount; ++i) {
        merge(arr, i * chank_size, (i + 1) * chank_size);
        if (i + 1 == chank_amount){
            cha
        }
    }

}*/

void parallelSort(int *arr, int arr_size, int chank_size, int threads_amount, int l, int r) {

    //CHANK SORT
    int chank_amount = floor(arr_size / (double)chank_size);
    for (int i = 0; i < chank_amount; ++i){
#pragma omp task firstprivate(i)
        mysort(arr, i * chank_size, (i + 1) * chank_size);
    }

    //MERGING
    //merge(arr, arr_size, chank_size, threads_amount, chank_amount);

    if (r - l > chank_size) {
        int m = l + (r - l) / 2;

        // Sort first and second halves
        parallelSort(arr, l, m);
        parallelSort(arr, m + 1, r);
    }else{
        merge(arr, l, m, r);
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
        parallelSort(arr, arr_size, chank_size, P);
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